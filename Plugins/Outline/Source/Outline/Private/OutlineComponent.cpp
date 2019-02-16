// Fill out your copyright notice in the Description page of Project Settings.

#include "OutlineComponent.h"
#include "PrimitiveSceneProxy.h"
//#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "Materials/MaterialInterface.h"


class FOutlineSceneProxy : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FOutlineSceneProxy(const UOutlineComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, bDraw(InComponent->bDraw)
		//, LineColor(InComponent->LineColor)
		//, bLineScreenSpace(InComponent->bLineScreenSpace)
		, LineThickness(InComponent->LineThickness)
		, Lines(InComponent->Lines)
		, LineMaterial(InComponent->LineMaterial)
	{
		bWillEverBeLit = false;
		bVerifyUsedMaterials = false;
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return(sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return(FPrimitiveSceneProxy::GetAllocatedSize() + Lines.GetAllocatedSize());
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		if (bDraw && Lines.Num() > 0)
		{
			/*for (const auto& elem : Lines)
			{
				Collector.GetPDI(0)->DrawLine(elem.Start, elem.End, FLinearColor(0.f, 1.f, 0.f), SDPG_World, LineThickness, 0.f, true);
			}*/

			const FMatrix& Transform = Views[0]->ViewMatrices.GetViewProjectionMatrix();
			const uint32 ViewportSizeX = Views[0]->UnscaledViewRect.Width();
			const uint32 ViewportSizeY = Views[0]->UnscaledViewRect.Height();

			FMatrix InvTransform = Transform.Inverse();
			FVector CameraX = InvTransform.TransformVector(FVector(1, 0, 0)).GetSafeNormal();
			FVector CameraY = InvTransform.TransformVector(FVector(0, 1, 0)).GetSafeNormal();
			FVector CameraZ = InvTransform.TransformVector(FVector(0, 0, 1)).GetSafeNormal();

			FDynamicMeshBuilder MeshBuilder(Views[0]->GetFeatureLevel());
			FColor VColor(1);

			for (int i = 0; i < Lines.Num(); ++i)
			{
				const float StartW = Transform.TransformFVector4(Lines[i].Start).W;
				const float EndW = Transform.TransformFVector4(Lines[i].End).W;

				const float ScalingStart = StartW / ViewportSizeX;
				const float ScalingEnd = EndW / ViewportSizeX;

				const float ScreenSpaceScaling = 2.0f;

				const float StartThickness = LineThickness * ScreenSpaceScaling * ScalingStart;
				const float EndThickness = LineThickness * ScreenSpaceScaling * ScalingEnd;

				const FVector WorldPointXS = CameraX * StartThickness * 0.5f;
				const FVector WorldPointYS = CameraY * StartThickness * 0.5f;

				const FVector WorldPointXE = CameraX * EndThickness * 0.5f;
				const FVector WorldPointYE = CameraY * EndThickness * 0.5f;

				// Generate vertices for the point such that the post-transform point size is constant.
				const FVector WorldPointX = CameraX * LineThickness * StartW / ViewportSizeX;
				const FVector WorldPointY = CameraY * LineThickness * StartW / ViewportSizeX;

				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].Start + WorldPointXS - WorldPointYS, FVector2D(1, 0), VColor));//0S
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].Start + WorldPointXS + WorldPointYS, FVector2D(1, 1), VColor));//1S
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].Start - WorldPointXS - WorldPointYS, FVector2D(0, 0), VColor));//2S
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].Start - WorldPointXS + WorldPointYS, FVector2D(0, 1), VColor));//3S

				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].End + WorldPointXE - WorldPointYE, FVector2D(1, 0), VColor));//0E
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].End + WorldPointXE + WorldPointYE, FVector2D(1, 1), VColor));//1E
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].End - WorldPointXE - WorldPointYE, FVector2D(0, 0), VColor));//2E
				MeshBuilder.AddVertex(FDynamicMeshVertex(Lines[i].End - WorldPointXE + WorldPointYE, FVector2D(0, 1), VColor));//3E

				//MeshBuilder.AddTriangle(i * 8, i * 8 + 1, i * 8 + 2);
				//MeshBuilder.AddTriangle(i * 8 + 1, i * 8 + 2, i * 8 + 3);
				//MeshBuilder.AddTriangle(i * 8 + 4, i * 8 + 5, i * 8 + 6);
				//MeshBuilder.AddTriangle(i * 8 + 5, i * 8 + 6, i * 8 + 7);
				MeshBuilder.AddTriangle(i * 8 + 2, i * 8 + 1, i * 8 + 6);
				MeshBuilder.AddTriangle(i * 8 + 1, i * 8 + 5, i * 8 + 6);
				MeshBuilder.AddTriangle(i * 8 + 3, i * 8, i * 8 + 7);
				MeshBuilder.AddTriangle(i * 8, i * 8 + 4, i * 8 + 7);
			}

			FMaterialRenderProxy* MaterialProxy = LineMaterial->GetRenderProxy(IsSelected());
			MeshBuilder.GetMesh(FMatrix::Identity, MaterialProxy, SDPG_World, true, false, 0, Collector);
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		return Result;
	}

private:
	bool bDraw;
	//FLinearColor LineColor;
	//bool bLineScreenSpace;
	float LineThickness;
	TArray<FSimpleLine> Lines;
	UMaterialInterface* LineMaterial;
};

FPrimitiveSceneProxy* UOutlineComponent::CreateSceneProxy()
{
	return new FOutlineSceneProxy(this);
}

FBoxSphereBounds UOutlineComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBox BBox(ForceInit);
	for (const FSimpleLine& Line : Lines)
	{
		BBox += Line.Start;
		BBox += Line.End;
	}
	return FBoxSphereBounds(BBox);
}
