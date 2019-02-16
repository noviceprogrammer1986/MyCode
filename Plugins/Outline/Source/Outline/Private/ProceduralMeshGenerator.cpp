// Fill out your copyright notice in the Description page of Project Settings.

#include "ProceduralMeshGenerator.h"
#include "ProceduralMeshComponent.h"
#include "FileManager.h"
#include "Components/StaticMeshComponent.h"
#include "Paths.h"

#include <fstream>
#include <sstream>
#include <string>

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
extern UNREALED_API UEditorEngine* GEditor;
#include "Engine/Selection.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#endif


// Sets default values
AProceduralMeshGenerator::AProceduralMeshGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
}

// Called when the game starts or when spawned
void AProceduralMeshGenerator::BeginPlay()
{
	Super::BeginPlay();

	Init();
}


void AProceduralMeshGenerator::Init()
{
	TArray<FString> ObjFiles;
	IFileManager::Get().FindFiles(ObjFiles, *(FPaths::ProjectContentDir() + ObjFilePath), TEXT("obj"));
	if (ObjFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No obj files found!"));
		return;
	}
	for (int32 i = 0; i < Outlines.Num(); ++i)
	{
		if (Outlines[i].RefActor == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no RefActor!"), i);
			continue;
		}
		if (Outlines[i].OutlineMaterial == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no Material!"), i);
			continue;
		}
		if (Outlines[i].ObjFilename.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has no ObjFilename!"), i);
			continue;
		}

		FString RealObjName;
		for (const FString& ObjName : ObjFiles)
		{
			if (ObjName == Outlines[i].ObjFilename || ObjName == Outlines[i].ObjFilename + ".obj")
			{
				RealObjName = ObjName;
				break;
			}
		}
		if (RealObjName.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Outlines[%d] has wrong ObjFilename!"), i);
			continue;
		}


		TArray<FVector> RawVertexArray;
		TArray<FVector4> RawIndexArray;

		FTransform RefTransform;
		if (Outlines[i].RefCompName.IsEmpty())
		{
			RefTransform = Outlines[i].RefActor->GetActorTransform();
		}
		else
		{
			for (const auto& Comp : Outlines[i].RefActor->GetComponentsByClass(USceneComponent::StaticClass()))
			{
				if (Comp->GetName().Contains(Outlines[i].RefCompName))
				{
					RefTransform = Cast<USceneComponent>(Comp)->GetComponentTransform();
					break;
				}
			}
		}

		std::ifstream InFile(TCHAR_TO_ANSI(*(FPaths::ProjectContentDir() + ObjFilePath + "/" + RealObjName)));

		for (std::string Line; std::getline(InFile, Line); )
		{
			std::istringstream LineStream(Line);
			std::string VF;
			LineStream >> VF;
			if (VF == "v")
			{
				float x, y, z;
				LineStream >> x >> y >> z;
				RawVertexArray.Add(RefTransform.TransformPosition(FVector(x, z, y)));
			}
			else if (VF == "f")
			{
				int32 x, y, z, w = 0;
				LineStream >> x >> y >> z >> w;
				RawIndexArray.Add(FVector4(x - 1, y - 1, z - 1, w - 1));
			}
		}

		UProceduralMeshComponent* OutlineComp = NewObject<UProceduralMeshComponent>(this);
		OutlineComp->RegisterComponent();
		Outlines[i].OutlineComp = OutlineComp;

		TArray<FLine> Lines;
		for (const auto& elem : RawIndexArray)
		{
			if (elem.W >= 0)
			{
				Lines.Add(FLine(RawVertexArray[int32(elem.X)], RawVertexArray[int32(elem.Y)]));
				Lines.Add(FLine(RawVertexArray[int32(elem.Y)], RawVertexArray[int32(elem.Z)]));
				Lines.Add(FLine(RawVertexArray[int32(elem.Z)], RawVertexArray[int32(elem.W)]));
				Lines.Add(FLine(RawVertexArray[int32(elem.W)], RawVertexArray[int32(elem.X)]));
			}
			else
			{
				Lines.Add(FLine(RawVertexArray[int32(elem.X)], RawVertexArray[int32(elem.Y)]));
				Lines.Add(FLine(RawVertexArray[int32(elem.Y)], RawVertexArray[int32(elem.Z)]));
				Lines.Add(FLine(RawVertexArray[int32(elem.Z)], RawVertexArray[int32(elem.X)]));
			}
		}

		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UV0; 
		TArray<FColor> VertexColors;
		TArray<FProcMeshTangent> Tangents;

		for (int j = 0; j < Lines.Num(); ++j)
		{
			FVector VLine = Lines[j].End - Lines[j].Start;
			float Max = FMath::Max3(FMath::Abs(VLine.X), FMath::Abs(VLine.Y), FMath::Abs(VLine.Z));
			if (FMath::Abs(VLine.X) == Max)
			{
				Vertices.Add(Lines[j].Start + FVector(0, Outlines[i].LineThickness, Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(0, -Outlines[i].LineThickness, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(0, Outlines[i].LineThickness, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(0, -Outlines[i].LineThickness, Outlines[i].LineThickness));

				Vertices.Add(Lines[j].End + FVector(0, Outlines[i].LineThickness, Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(0, -Outlines[i].LineThickness, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(0, Outlines[i].LineThickness, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(0, -Outlines[i].LineThickness, Outlines[i].LineThickness));

				Normals.Add(FVector(0, 0.7071, 0.7071));
				Normals.Add(FVector(0, -0.7071, -0.7071));
				Normals.Add(FVector(0, 0.7071, -0.7071));
				Normals.Add(FVector(0, -0.7071, 0.7071));

				Normals.Add(FVector(0, 0.7071, 0.7071));
				Normals.Add(FVector(0, -0.7071, -0.7071));
				Normals.Add(FVector(0, 0.7071, -0.7071));
				Normals.Add(FVector(0, -0.7071, 0.7071));
			}
			else if (FMath::Abs(VLine.Y) == Max)
			{
				Vertices.Add(Lines[j].Start + FVector(Outlines[i].LineThickness, 0, Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(-Outlines[i].LineThickness, 0, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(Outlines[i].LineThickness, 0, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].Start + FVector(-Outlines[i].LineThickness, 0, Outlines[i].LineThickness));

				Vertices.Add(Lines[j].End + FVector(Outlines[i].LineThickness, 0, Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(-Outlines[i].LineThickness, 0, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(Outlines[i].LineThickness, 0, -Outlines[i].LineThickness));
				Vertices.Add(Lines[j].End + FVector(-Outlines[i].LineThickness, 0, Outlines[i].LineThickness));

				Normals.Add(FVector(0.7071, 0, 0.7071));
				Normals.Add(FVector(-0.7071, 0, -0.7071));
				Normals.Add(FVector(0.7071, 0, -0.7071));
				Normals.Add(FVector(-0.7071, 0, 0.7071));

				Normals.Add(FVector(0.7071, 0, 0.7071));
				Normals.Add(FVector(-0.7071, 0, -0.7071));
				Normals.Add(FVector(0.7071, 0, -0.7071));
				Normals.Add(FVector(-0.7071, 0, 0.7071));
			}
			else
			{
				Vertices.Add(Lines[j].Start + FVector(Outlines[i].LineThickness, Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].Start + FVector(-Outlines[i].LineThickness, -Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].Start + FVector(Outlines[i].LineThickness, -Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].Start + FVector(-Outlines[i].LineThickness, Outlines[i].LineThickness, 0));

				Vertices.Add(Lines[j].End + FVector(Outlines[i].LineThickness, Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].End + FVector(-Outlines[i].LineThickness, -Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].End + FVector(Outlines[i].LineThickness, -Outlines[i].LineThickness, 0));
				Vertices.Add(Lines[j].End + FVector(-Outlines[i].LineThickness, Outlines[i].LineThickness, 0));

				Normals.Add(FVector(0.7071, 0.7071, 0));
				Normals.Add(FVector(-0.7071, -0.7071, 0));
				Normals.Add(FVector(0.7071, -0.7071, 0));
				Normals.Add(FVector(-0.7071, 0.7071, 0));

				Normals.Add(FVector(0.7071, 0.7071, 0));
				Normals.Add(FVector(-0.7071, -0.7071, 0));
				Normals.Add(FVector(0.7071, -0.7071, 0));
				Normals.Add(FVector(-0.7071, 0.7071, 0));
			}

			Triangles.Add(j * 8);
			Triangles.Add(j * 8 + 1);
			Triangles.Add(j * 8 + 5);
			Triangles.Add(j * 8);
			Triangles.Add(j * 8 + 5);
			Triangles.Add(j * 8 + 4);
			Triangles.Add(j * 8 + 2);
			Triangles.Add(j * 8 + 3);
			Triangles.Add(j * 8 + 7);
			Triangles.Add(j * 8 + 2);
			Triangles.Add(j * 8 + 7);
			Triangles.Add(j * 8 + 6);

		}

		OutlineComp->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);
		OutlineComp->SetMaterial(0, Outlines[i].OutlineMaterial);

		if (Outlines[i].RefCompName.IsEmpty())
		{
			OutlineComp->AttachToComponent(Outlines[i].RefActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}
		else
		{
			for (const auto& Comp : Outlines[i].RefActor->GetComponentsByClass(USceneComponent::StaticClass()))
			{
				if (Comp->GetName().Contains(Outlines[i].RefCompName))
				{
					OutlineComp->AttachToComponent(Cast<USceneComponent>(Comp), FAttachmentTransformRules::KeepWorldTransform);
					break;
				}
			}
		}
		
		OutlineComp->SetVisibility(Outlines[i].bDraw);
	}
}

void AProceduralMeshGenerator::ShowOutline(bool bVisible, AActor* InActor)
{
	if (InActor)
	{
		for (const auto& elem : Outlines)
		{
			if (elem.OutlineComp && elem.RefActor == InActor)
			{
				elem.OutlineComp->SetVisibility(bVisible);
			}
		}
	}
	else
	{
		for (const auto& elem : Outlines)
		{
			if (elem.OutlineComp)
			{
				elem.OutlineComp->SetVisibility(bVisible);
			}
		}
	}
}

#if WITH_EDITOR
void AProceduralMeshGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == FName("SearchName"))
	{
		for (int32 i = 0; i < Outlines.Num(); ++i)
		{
			if (SearchName.Contains(Outlines[i].RefCompName))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s:[%d]"), *SearchName, i);
				return;
			}
		}

		for (int32 i = 0; i < Outlines.Num(); ++i)
		{
			if (Outlines[i].RefActor->GetName().Contains(SearchName))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s:[%d]"), *SearchName, i);
			}
		}
	}
}

void AProceduralMeshGenerator::AutoFill()
{
	TArray<FString> ObjFiles;
	IFileManager::Get().FindFiles(ObjFiles, *(FPaths::ProjectContentDir() + ObjFilePath), TEXT("obj"));
	if (ObjFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No obj files found!"));
		return;
	}
	if (DefaultOutlineMaterial == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Please set default material!"));
		return;
	}

	for (int32 i = 0; i < GEditor->GetSelectedActorCount(); ++i)
	{
		AStaticMeshActor* MyActor = Cast<AStaticMeshActor>(GEditor->GetSelectedActors()->GetSelectedObject(i));
		if (MyActor)
		{
			bool bNewActor = true;
			for (const auto& Outline : Outlines)
			{
				if (MyActor == Outline.RefActor)
				{
					UE_LOG(LogTemp, Warning, TEXT("[%s] already added"), *MyActor->GetName());
					bNewActor = false;
					break;
				}
			}

			if (bNewActor)
			{
				//GEditor->AddOnScreenDebugMessage(-1, 4.0, FColor::Green, MyActor->GetStaticMeshComponent()->GetStaticMesh()->GetName());
				FString MeshName = MyActor->GetStaticMeshComponent()->GetStaticMesh()->GetName();
				if (ObjFiles.Contains(MeshName + ".obj"))
				{
					FOutline NewOutline;
					NewOutline.RefActor = MyActor;
					NewOutline.ObjFilename = MeshName;
					NewOutline.OutlineMaterial = DefaultOutlineMaterial;
					Outlines.Add(NewOutline);
				}
			}
		}
	}
}
#endif