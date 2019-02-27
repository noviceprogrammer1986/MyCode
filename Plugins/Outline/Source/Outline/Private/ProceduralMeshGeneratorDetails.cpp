#include "ProceduralMeshGeneratorDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "SButton.h"
#include "STextBlock.h"
#include "ProceduralMeshGenerator.h"
#include "ProceduralMeshComponent.h"
#include "RawMesh.h"
#include "Engine/StaticMesh.h"
#include "AssetRegistryModule.h"


#define LOCTEXT_NAMESPACE "ProceduralMeshGeneratorDetails"

TSharedRef<IDetailCustomization> FProceduralMeshGeneratorDetails::MakeInstance()
{
	return MakeShareable(new FProceduralMeshGeneratorDetails);
}

void FProceduralMeshGeneratorDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& CategoryBuilder = DetailBuilder.EditCategory("Button");
	const FText ConvertToStaticMeshText = LOCTEXT("ConvertToStaticMesh", "Create StaticMesh");

	Generator = Cast<AProceduralMeshGenerator>(DetailBuilder.GetSelectedObjects()[0]);

	CategoryBuilder.AddCustomRow(ConvertToStaticMeshText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
		.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
			.ToolTipText(LOCTEXT("ConvertToStaticMeshTooltip", "Hello"))
			.OnClicked(this, &FProceduralMeshGeneratorDetails::ClickMe)
			.IsEnabled(!bIsConverting)
			.Content()
			[
				SNew(STextBlock)
				.Text(ConvertToStaticMeshText)
			]
		];
}

FReply FProceduralMeshGeneratorDetails::ClickMe()
{
	bIsConverting = true;

	if (Generator.IsValid())
	{
		Generator->Init();

		for (auto& elem : Generator->Outlines)
		{
			if (elem.OutlineComp != nullptr)
			{
				FString UserPackageName = FString(TEXT("/Game/_OutlineMesh/")) + elem.ObjFilename;
				FName MeshName = *elem.ObjFilename;

				// Raw mesh data we are filling in
				FRawMesh RawMesh;
				// Materials to apply to new mesh
				TArray<UMaterialInterface*> MeshMaterials;

				const int32 NumSections = elem.OutlineComp->GetNumSections();
				int32 VertexBase = 0;
				for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
				{
					FProcMeshSection* ProcSection = elem.OutlineComp->GetProcMeshSection(SectionIdx);

					// Copy verts
					for (FProcMeshVertex& Vert : ProcSection->ProcVertexBuffer)
					{
						RawMesh.VertexPositions.Add(Vert.Position);
					}

					// Copy 'wedge' info
					int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
					for (int32 IndexIdx = 0; IndexIdx < NumIndices; IndexIdx++)
					{
						int32 Index = ProcSection->ProcIndexBuffer[IndexIdx];

						RawMesh.WedgeIndices.Add(Index + VertexBase);

						FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[Index];

						FVector TangentX = ProcVertex.Tangent.TangentX;
						FVector TangentZ = ProcVertex.Normal;
						FVector TangentY = (TangentX ^ TangentZ).GetSafeNormal() * (ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f);

						RawMesh.WedgeTangentX.Add(TangentX);
						RawMesh.WedgeTangentY.Add(TangentY);
						RawMesh.WedgeTangentZ.Add(TangentZ);

						RawMesh.WedgeTexCoords[0].Add(ProcVertex.UV0);
						RawMesh.WedgeTexCoords[1].Add(ProcVertex.UV1);
						RawMesh.WedgeTexCoords[2].Add(ProcVertex.UV2);
						RawMesh.WedgeTexCoords[3].Add(ProcVertex.UV3);
						RawMesh.WedgeColors.Add(ProcVertex.Color);
					}

					// copy face info
					int32 NumTris = NumIndices / 3;
					for (int32 TriIdx = 0; TriIdx < NumTris; TriIdx++)
					{
						RawMesh.FaceMaterialIndices.Add(SectionIdx);
						RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
					}

					// Remember material
					MeshMaterials.Add(elem.OutlineComp->GetMaterial(SectionIdx));

					// Update offset for creating one big index/vertex buffer
					VertexBase += ProcSection->ProcVertexBuffer.Num();
				}

				// If we got some valid data.
				if (RawMesh.VertexPositions.Num() >= 3 && RawMesh.WedgeIndices.Num() >= 3)
				{
					// Then find/create it.
					UPackage* Package = CreatePackage(NULL, *UserPackageName);
					check(Package);

					// Create StaticMesh object
					UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
					StaticMesh->InitResources();

					StaticMesh->LightingGuid = FGuid::NewGuid();

					// Add source to new StaticMesh
					FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
					SrcModel.BuildSettings.bRecomputeNormals = false;
					SrcModel.BuildSettings.bRecomputeTangents = false;
					SrcModel.BuildSettings.bRemoveDegenerates = false;
					SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
					SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
					SrcModel.BuildSettings.bGenerateLightmapUVs = true;
					SrcModel.BuildSettings.SrcLightmapIndex = 0;
					SrcModel.BuildSettings.DstLightmapIndex = 1;
					SrcModel.SaveRawMesh(RawMesh);

					// Copy materials to new mesh
					for (UMaterialInterface* Material : MeshMaterials)
					{
						StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
					}

					//Set the Imported version before calling the build
					StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

					// Build mesh from source
					StaticMesh->Build(false);
					StaticMesh->PostEditChange();

					// Notify asset registry of new asset
					FAssetRegistryModule::AssetCreated(StaticMesh);
				}

				elem.OutlineComp->DestroyComponent();
			}
		}
	}

	bIsConverting = false;
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE