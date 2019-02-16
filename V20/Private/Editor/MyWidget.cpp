// Fill out your copyright notice in the Description page of Project Settings.

#include "MyWidget.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DynamicMeshBuilder.h"
#include "Engine/Engine.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "MyGameViewportClient.h"


IMPLEMENT_HIT_PROXY(HMyWidgetAxis, HHitProxy);

static const float AXIS_LENGTH = 35.0f;
static const int32 AXIS_CIRCLE_SIDES = 24;
static const float INNER_AXIS_CIRCLE_RADIUS = 48.0f;
static const float OUTER_AXIS_CIRCLE_RADIUS = 56.0f;
static const float ROTATION_TEXT_RADIUS = 75.0f;
static const uint8 LargeInnerAlpha = 0x3f;
static const uint8 SmallInnerAlpha = 0x0f;
static const uint8 LargeOuterAlpha = 0x7f;
static const uint8 SmallOuterAlpha = 0x0f;

FMyWidget::FMyWidget(UMyGameViewportClient* InMyViewportClient)
	: MyViewportClient(InMyViewportClient)
{
	CustomCoordSystem = FMatrix::Identity;
	CustomCoordSystemSpace = ECoordSystem::COORD_World;
	WidgetLocation = FVector::ZeroVector;

	AxisColorX = FLinearColor(0.594f, 0.0197f, 0.0f);
	AxisColorY = FLinearColor(0.1349f, 0.3959f, 0.0f);
	AxisColorZ = FLinearColor(0.0251f, 0.207f, 0.85f);
	CurrentColor = FColor::Yellow;
	AxisMaterialBase = (UMaterial*)StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/_GizmoMaterials/GizmoMaterial.GizmoMaterial"), NULL, LOAD_None, NULL);
	AxisMaterialX = UMaterialInstanceDynamic::Create(AxisMaterialBase, NULL);
	AxisMaterialX->SetVectorParameterValue("GizmoColor", AxisColorX);
	AxisMaterialY = UMaterialInstanceDynamic::Create(AxisMaterialBase, NULL);
	AxisMaterialY->SetVectorParameterValue("GizmoColor", AxisColorY);
	AxisMaterialZ = UMaterialInstanceDynamic::Create(AxisMaterialBase, NULL);
	AxisMaterialZ->SetVectorParameterValue("GizmoColor", AxisColorZ);
	CurrentAxisMaterial = UMaterialInstanceDynamic::Create(AxisMaterialBase, NULL);
	CurrentAxisMaterial->SetVectorParameterValue("GizmoColor", CurrentColor);
	OpaquePlaneMaterialXY = UMaterialInstanceDynamic::Create(AxisMaterialBase, NULL);
	OpaquePlaneMaterialXY->SetVectorParameterValue("GizmoColor", FLinearColor::White);
	TransparentPlaneMaterialXY = (UMaterial*)StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/_GizmoMaterials/WidgetVertexColorMaterial.WidgetVertexColorMaterial"), NULL, LOAD_None, NULL);
	GridMaterial = (UMaterial*)StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/_GizmoMaterials/WidgetGridVertexColorMaterial_Ma.WidgetGridVertexColorMaterial_Ma"), NULL, LOAD_None, NULL);
}

void FMyWidget::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(AxisMaterialBase);
	Collector.AddReferencedObject(AxisMaterialX);
	Collector.AddReferencedObject(AxisMaterialY);
	Collector.AddReferencedObject(AxisMaterialZ);
	Collector.AddReferencedObject(OpaquePlaneMaterialXY);
	Collector.AddReferencedObject(TransparentPlaneMaterialXY);
	Collector.AddReferencedObject(GridMaterial);
	Collector.AddReferencedObject(CurrentAxisMaterial);
}

void FMyWidget::Render(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (CustomCoordSystemSpace == COORD_Local && MyViewportClient->SelectedActor)
	{
		CustomCoordSystem = FQuatRotationMatrix(MyViewportClient->SelectedActor->GetActorQuat());
		if (!CustomCoordSystem.Equals(FMatrix::Identity))
		{
			CustomCoordSystem.RemoveScaling();
		}
	}
	else
	{
		CustomCoordSystem = FMatrix::Identity;
	}

	switch (MyViewportClient->WidgetMode)
	{
	case WM_Translate:
	{
		Render_Translate(View, PDI, WidgetLocation, true);
	}
	break;

	case WM_Rotate:
	{
		Render_Rotate(View, PDI, WidgetLocation, true);
	}
	break;

	case WM_Scale:
	{
		Render_Scale(View, PDI, WidgetLocation, true);
	}
	break;

	default:
		break;
	}

}

void DrawCornerHelper(FPrimitiveDrawInterface* PDI, const FMatrix& LocalToWorld, const FVector& Length, float Thickness, const FMaterialRenderProxy* MaterialRenderProxy, uint8 DepthPriorityGroup)
{
	const float TH = Thickness;

	float TX = Length.X / 2;
	float TY = Length.Y / 2;
	float TZ = Length.Z / 2;

	FDynamicMeshBuilder MeshBuilder(PDI->View->GetFeatureLevel());

	// Top
	{
		int32 VertexIndices[4];
		VertexIndices[0] = MeshBuilder.AddVertex(FVector(-TX, -TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);
		VertexIndices[1] = MeshBuilder.AddVertex(FVector(-TX, +TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);
		VertexIndices[2] = MeshBuilder.AddVertex(FVector(+TX, +TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);
		VertexIndices[3] = MeshBuilder.AddVertex(FVector(+TX, -TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), FColor::White);

		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[3]);
	}

	//Left
	{
		int32 VertexIndices[4];
		VertexIndices[0] = MeshBuilder.AddVertex(FVector(-TX, -TY, TZ - TH), FVector2D::ZeroVector, FVector(0, 0, 1), FVector(0, 1, 0), FVector(-1, 0, 0), FColor::White);
		VertexIndices[1] = MeshBuilder.AddVertex(FVector(-TX, -TY, TZ), FVector2D::ZeroVector, FVector(0, 0, 1), FVector(0, 1, 0), FVector(-1, 0, 0), FColor::White);
		VertexIndices[2] = MeshBuilder.AddVertex(FVector(-TX, +TY, TZ), FVector2D::ZeroVector, FVector(0, 0, 1), FVector(0, 1, 0), FVector(-1, 0, 0), FColor::White);
		VertexIndices[3] = MeshBuilder.AddVertex(FVector(-TX, +TY, TZ - TH), FVector2D::ZeroVector, FVector(0, 0, 1), FVector(0, 1, 0), FVector(-1, 0, 0), FColor::White);


		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[3]);
	}

	// Front
	{
		int32 VertexIndices[5];
		VertexIndices[0] = MeshBuilder.AddVertex(FVector(-TX, +TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 1, 0), FColor::White);
		VertexIndices[1] = MeshBuilder.AddVertex(FVector(-TX, +TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 1, 0), FColor::White);
		VertexIndices[2] = MeshBuilder.AddVertex(FVector(+TX - TH, +TY, +TX), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 1, 0), FColor::White);
		VertexIndices[3] = MeshBuilder.AddVertex(FVector(+TX, +TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 1, 0), FColor::White);
		VertexIndices[4] = MeshBuilder.AddVertex(FVector(+TX - TH, +TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 1, 0), FColor::White);

		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[4]);
		MeshBuilder.AddTriangle(VertexIndices[4], VertexIndices[2], VertexIndices[3]);
	}

	// Back
	{
		int32 VertexIndices[5];
		VertexIndices[0] = MeshBuilder.AddVertex(FVector(-TX, -TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, 1), FVector(0, -1, 0), FColor::White);
		VertexIndices[1] = MeshBuilder.AddVertex(FVector(-TX, -TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, 1), FVector(0, -1, 0), FColor::White);
		VertexIndices[2] = MeshBuilder.AddVertex(FVector(+TX - TH, -TY, +TX), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, 1), FVector(0, -1, 0), FColor::White);
		VertexIndices[3] = MeshBuilder.AddVertex(FVector(+TX, -TY, +TZ), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, 1), FVector(0, -1, 0), FColor::White);
		VertexIndices[4] = MeshBuilder.AddVertex(FVector(+TX - TH, -TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, 1), FVector(0, -1, 0), FColor::White);

		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[4]);
		MeshBuilder.AddTriangle(VertexIndices[4], VertexIndices[2], VertexIndices[3]);
	}
	// Bottom
	{
		int32 VertexIndices[4];
		VertexIndices[0] = MeshBuilder.AddVertex(FVector(-TX, -TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 0, 1), FColor::White);
		VertexIndices[1] = MeshBuilder.AddVertex(FVector(-TX, +TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 0, 1), FColor::White);
		VertexIndices[2] = MeshBuilder.AddVertex(FVector(+TX - TH, +TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 0, 1), FColor::White);
		VertexIndices[3] = MeshBuilder.AddVertex(FVector(+TX - TH, -TY, TZ - TH), FVector2D::ZeroVector, FVector(1, 0, 0), FVector(0, 0, -1), FVector(0, 0, 1), FColor::White);

		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
		MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[3]);
	}
	MeshBuilder.Draw(PDI, LocalToWorld, MaterialRenderProxy, DepthPriorityGroup, 0.f);
}

void DrawDualAxis(FPrimitiveDrawInterface* PDI, const FMatrix& BoxToWorld, const FVector& Length, float Thickness, const FMaterialRenderProxy* AxisMat, const FMaterialRenderProxy* Axis2Mat)
{
	DrawCornerHelper(PDI, BoxToWorld, Length, Thickness, Axis2Mat, SDPG_Foreground);
	DrawCornerHelper(PDI, FScaleMatrix(FVector(-1, 1, 1)) * FRotationMatrix(FRotator(-90, 0, 0)) * BoxToWorld, Length, Thickness, AxisMat, SDPG_Foreground);
}

void FMyWidget::Render_Cube(FPrimitiveDrawInterface* PDI, const FMatrix& InMatrix, const UMaterialInterface* InMaterial, const FVector& InScale)
{
	const FMatrix CubeToWorld = FScaleMatrix(InScale) * InMatrix;
	DrawBox(PDI, CubeToWorld, FVector(1, 1, 1), InMaterial->GetRenderProxy(false), SDPG_Foreground);
}

void FMyWidget::Render_Axis(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, FMatrix& InMatrix, UMaterialInterface* InMaterial, const FLinearColor& InColor, FVector2D& OutAxisDir, const FVector& InScale, bool bDrawWidget, bool bCubeHead)
{
	FMatrix AxisRotation = FMatrix::Identity;
	if (InAxis == EAxisList::Y)
	{
		AxisRotation = FRotationMatrix::MakeFromXZ(FVector(0, 1, 0), FVector(0, 0, 1));
	}
	else if (InAxis == EAxisList::Z)
	{
		AxisRotation = FRotationMatrix::MakeFromXY(FVector(0, 0, 1), FVector(0, 1, 0));
	}

	FMatrix ArrowToWorld = AxisRotation * InMatrix;

	// The scale that is passed in potentially leaves one component with a scale of 1, if that happens
	// we need to extract the inform scale and use it to construct the scale that transforms the primitives
	float UniformScale = InScale.GetMax() > 1.0f ? InScale.GetMax() : InScale.GetMin() < 1.0f ? InScale.GetMin() : 1.0f;
	// After the primitives have been scaled and transformed, we apply this inverse scale that flattens the dimension
	// that was scaled up to prevent it from intersecting with the near plane.  In perspective this won't have any effect,
	// but in the ortho viewports it will prevent scaling in the direction of the camera and thus intersecting the near plane.
	FVector FlattenScale = FVector(InScale.Component(0) == 1.0f ? 1.0f / UniformScale : 1.0f, InScale.Component(1) == 1.0f ? 1.0f / UniformScale : 1.0f, InScale.Component(2) == 1.0f ? 1.0f / UniformScale : 1.0f);

	FScaleMatrix Scale(UniformScale);
	ArrowToWorld = Scale * ArrowToWorld;

	if (bDrawWidget)
	{
		const bool bDisabled = false;
		PDI->SetHitProxy(new HMyWidgetAxis(InAxis, bDisabled));

		const float AxisLength = AXIS_LENGTH;
		const float HalfHeight = AxisLength / 2.0f;
		const float CylinderRadius = 1.2f;
		const FVector Offset(0, 0, HalfHeight);

		switch (InAxis)
		{
		case EAxisList::X:
		{
			DrawCylinder(PDI, (Scale * FRotationMatrix(FRotator(-90, 0.f, 0)) * InMatrix) * FScaleMatrix(FlattenScale), Offset, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), CylinderRadius, HalfHeight, 16, InMaterial->GetRenderProxy(false), SDPG_Foreground);
			break;
		}
		case EAxisList::Y:
		{
			DrawCylinder(PDI, (Scale * FRotationMatrix(FRotator(0, 0, 90)) * InMatrix)* FScaleMatrix(FlattenScale), Offset, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), CylinderRadius, HalfHeight, 16, InMaterial->GetRenderProxy(false), SDPG_Foreground);
			break;
		}
		case EAxisList::Z:
		{
			DrawCylinder(PDI, (Scale * InMatrix) * FScaleMatrix(FlattenScale), Offset, FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1), CylinderRadius, HalfHeight, 16, InMaterial->GetRenderProxy(false), SDPG_Foreground);
			break;
		}
		}

		if (bCubeHead)
		{
			const float CubeHeadOffset = 3.0f;
			FVector RootPos(AxisLength + CubeHeadOffset, 0, 0);

			Render_Cube(PDI, (FTranslationMatrix(RootPos) * ArrowToWorld) * FScaleMatrix(FlattenScale), InMaterial, FVector(4.0f));
		}
		else
		{
			const float ConeHeadOffset = 12.0f;
			FVector RootPos(AxisLength + ConeHeadOffset, 0, 0);

			float Angle = FMath::DegreesToRadians(PI * 5);
			DrawCone(PDI, (FScaleMatrix(-13) * FTranslationMatrix(RootPos) * ArrowToWorld) * FScaleMatrix(FlattenScale), Angle, Angle, 32, false, FColor::White, InMaterial->GetRenderProxy(false), SDPG_Foreground);
		}

		PDI->SetHitProxy(NULL);
	}

	FVector2D NewOrigin;
	FVector2D AxisEnd;
	const FVector AxisEndWorld = ArrowToWorld.TransformPosition(FVector(64, 0, 0));
	const FVector WidgetOrigin = InMatrix.GetOrigin();

	if (View->ScreenToPixel(View->WorldToScreen(WidgetOrigin), NewOrigin) &&
		View->ScreenToPixel(View->WorldToScreen(AxisEndWorld), AxisEnd))
	{
		// If both the origin and the axis endpoint are in front of the camera, trivially calculate the viewport space axis direction
		OutAxisDir = (AxisEnd - NewOrigin).GetSafeNormal();
	}
	else
	{
		// If either the origin or axis endpoint are behind the camera, translate the entire widget in front of the camera in the view direction before performing the
		// viewport space calculation
		const FMatrix InvViewMatrix = View->ViewMatrices.GetInvViewMatrix();
		const FVector ViewLocation = InvViewMatrix.GetOrigin();
		const FVector ViewDirection = InvViewMatrix.GetUnitAxis(EAxis::Z);
		const FVector Offset = ViewDirection * (FVector::DotProduct(ViewLocation - WidgetOrigin, ViewDirection) + 100.0f);
		const FVector AdjustedWidgetOrigin = WidgetOrigin + Offset;
		const FVector AdjustedWidgetAxisEnd = AxisEndWorld + Offset;

		if (View->ScreenToPixel(View->WorldToScreen(AdjustedWidgetOrigin), NewOrigin) &&
			View->ScreenToPixel(View->WorldToScreen(AdjustedWidgetAxisEnd), AxisEnd))
		{
			OutAxisDir = -(AxisEnd - NewOrigin).GetSafeNormal();
		}
	}
}

void FMyWidget::Render_Translate(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget)
{
	// Figure out axis colors
	const FLinearColor& XColor = (CurrentAxis&EAxisList::X ? (FLinearColor)CurrentColor : AxisColorX);
	const FLinearColor& YColor = (CurrentAxis&EAxisList::Y ? (FLinearColor)CurrentColor : AxisColorY);
	const FLinearColor& ZColor = (CurrentAxis&EAxisList::Z ? (FLinearColor)CurrentColor : AxisColorZ);

	// Figure out axis matrices
	FMatrix WidgetMatrix = CustomCoordSystem * FTranslationMatrix(InLocation);
	const EAxisList::Type DrawAxis = EAxisList::Type::XYZ;

	float UniformScale = View->WorldToScreen(InLocation).W * (4.0f / View->UnscaledViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);
	FVector MyScale(UniformScale, UniformScale, UniformScale);

	UMaterialInterface* XMaterial = (CurrentAxis&EAxisList::X ? CurrentAxisMaterial : AxisMaterialX);
	Render_Axis(View, PDI, EAxisList::X, WidgetMatrix, XMaterial, XColor, XAxisDir, MyScale, bDrawWidget);

	UMaterialInterface* YMaterial = (CurrentAxis&EAxisList::Y ? CurrentAxisMaterial : AxisMaterialY);
	Render_Axis(View, PDI, EAxisList::Y, WidgetMatrix, YMaterial, YColor, YAxisDir, MyScale, bDrawWidget);

	UMaterialInterface* ZMaterial = (CurrentAxis&EAxisList::Z ? CurrentAxisMaterial : AxisMaterialZ);
	Render_Axis(View, PDI, EAxisList::Z, WidgetMatrix, ZMaterial, ZColor, ZAxisDir, MyScale, bDrawWidget);

	// Draw the grabbers
	if (bDrawWidget)
	{
		FVector CornerPos = FVector(7, 0, 7) * UniformScale;
		FVector AxisSize = FVector(12, 1.2, 12) * UniformScale;
		float CornerLength = 1.2f * UniformScale;

		// After the primitives have been scaled and transformed, we apply this inverse scale that flattens the dimension
		// that was scaled up to prevent it from intersecting with the near plane.  In perspective this won't have any effect,
		// but in the ortho viewports it will prevent scaling in the direction of the camera and thus intersecting the near plane.
		FVector FlattenScale = FVector(1.0f);

		if ((DrawAxis&EAxisList::XY) == EAxisList::XY)
		{
			UMaterialInterface* XMaterial = ((CurrentAxis&EAxisList::XY) == EAxisList::XY ? CurrentAxisMaterial : AxisMaterialX);
			UMaterialInterface* YMaterial = ((CurrentAxis&EAxisList::XY) == EAxisList::XY ? CurrentAxisMaterial : AxisMaterialY);

			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::XY));
			{
				DrawDualAxis(PDI, (FTranslationMatrix(CornerPos) * FRotationMatrix(FRotator(0, 0, 90)) * WidgetMatrix) * FScaleMatrix(FlattenScale), AxisSize, CornerLength, XMaterial->GetRenderProxy(false), YMaterial->GetRenderProxy(false));
			}
			PDI->SetHitProxy(NULL);
		}

		if ((DrawAxis&EAxisList::XZ) == EAxisList::XZ)
		{
			UMaterialInterface* XMaterial = ((CurrentAxis&EAxisList::XZ) == EAxisList::XZ ? CurrentAxisMaterial : AxisMaterialX);
			UMaterialInterface* ZMaterial = ((CurrentAxis&EAxisList::XZ) == EAxisList::XZ ? CurrentAxisMaterial : AxisMaterialZ);

			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::XZ));
			{
				DrawDualAxis(PDI, (FTranslationMatrix(CornerPos) * WidgetMatrix) * FScaleMatrix(FlattenScale), AxisSize, CornerLength, XMaterial->GetRenderProxy(false), ZMaterial->GetRenderProxy(false));
			}
			PDI->SetHitProxy(NULL);
		}

		if ((DrawAxis&EAxisList::YZ) == EAxisList::YZ)
		{
			UMaterialInterface* YMaterial = ((CurrentAxis&EAxisList::YZ) == EAxisList::YZ ? CurrentAxisMaterial : AxisMaterialY);
			UMaterialInterface* ZMaterial = ((CurrentAxis&EAxisList::YZ) == EAxisList::YZ ? CurrentAxisMaterial : AxisMaterialZ);

			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::YZ));
			{
				DrawDualAxis(PDI, (FTranslationMatrix(CornerPos) * FRotationMatrix(FRotator(0, 90, 0)) * WidgetMatrix) * FScaleMatrix(FlattenScale), AxisSize, CornerLength, YMaterial->GetRenderProxy(false), ZMaterial->GetRenderProxy(false));
			}
			PDI->SetHitProxy(NULL);
		}
	}
}

void FMyWidget::DrawThickArc(const FThickArcParams& InParams, const FVector& Axis0, const FVector& Axis1, const float InStartAngle, const float InEndAngle, const FColor& InColor)
{
	if (InColor.A == 0)
	{
		return;
	}

	// Add more sides to the circle if we've been scaled up to keep the circle looking circular
	// An extra side for every 5 extra unreal units seems to produce a nice result
	const int32 CircleSides = AXIS_CIRCLE_SIDES;
	const int32 NumPoints = FMath::TruncToInt(CircleSides * (InEndAngle - InStartAngle) / (PI / 2)) + 1;

	FColor TriangleColor = InColor;
	FColor RingColor = InColor;
	RingColor.A = MAX_uint8;

	FVector ZAxis = Axis0 ^ Axis1;
	FVector LastVertex;

	FDynamicMeshBuilder MeshBuilder(InParams.PDI->View->GetFeatureLevel());

	for (int32 RadiusIndex = 0; RadiusIndex < 2; ++RadiusIndex)
	{
		float Radius = (RadiusIndex == 0) ? InParams.OuterRadius : InParams.InnerRadius;
		float TCRadius = Radius / (float)InParams.OuterRadius;
		//Compute vertices for base circle.
		for (int32 VertexIndex = 0; VertexIndex <= NumPoints; VertexIndex++)
		{
			float Percent = VertexIndex / (float)NumPoints;
			float Angle = FMath::Lerp(InStartAngle, InEndAngle, Percent);
			float AngleDeg = FRotator::ClampAxis(Angle * 180.f / PI);

			FVector VertexDir = Axis0.RotateAngleAxis(AngleDeg, ZAxis);
			VertexDir.Normalize();

			float TCAngle = Percent * (PI / 2);
			FVector2D TC(TCRadius*FMath::Cos(Angle), TCRadius*FMath::Sin(Angle));

			const FVector VertexPosition = InParams.Position + VertexDir * Radius;
			FVector Normal = VertexPosition - InParams.Position;
			Normal.Normalize();

			FDynamicMeshVertex MeshVertex;
			MeshVertex.Position = VertexPosition;
			MeshVertex.Color = TriangleColor;
			MeshVertex.TextureCoordinate[0] = TC;

			MeshVertex.SetTangents(
				-ZAxis,
				(-ZAxis) ^ Normal,
				Normal
			);

			MeshBuilder.AddVertex(MeshVertex); //Add bottom vertex

			// Push out the arc line borders so they dont z-fight with the mesh arcs
			FVector StartLinePos = LastVertex;
			FVector EndLinePos = VertexPosition;
			if (VertexIndex != 0)
			{
				InParams.PDI->DrawLine(StartLinePos, EndLinePos, RingColor, SDPG_Foreground);
			}
			LastVertex = VertexPosition;
		}
	}

	//Add top/bottom triangles, in the style of a fan.
	int32 InnerVertexStartIndex = NumPoints + 1;
	for (int32 VertexIndex = 0; VertexIndex < NumPoints; VertexIndex++)
	{
		MeshBuilder.AddTriangle(VertexIndex, VertexIndex + 1, InnerVertexStartIndex + VertexIndex);
		MeshBuilder.AddTriangle(VertexIndex + 1, InnerVertexStartIndex + VertexIndex + 1, InnerVertexStartIndex + VertexIndex);
	}

	MeshBuilder.Draw(InParams.PDI, FMatrix::Identity, InParams.Material->GetRenderProxy(false), SDPG_Foreground, 0.f);
}

void FMyWidget::DrawPartialRotationArc(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const float InStartAngle, const float InEndAngle, const FColor& InColor, const float InScale)
{
	const float InnerRadius = INNER_AXIS_CIRCLE_RADIUS * InScale;
	const float OuterRadius = OUTER_AXIS_CIRCLE_RADIUS * InScale;

	bool bIsPerspective = (View->ViewMatrices.GetProjectionMatrix().M[3][3] < 1.0f);
	PDI->SetHitProxy(new HMyWidgetAxis(InAxis));
	{
		FThickArcParams OuterArcParams(PDI, InLocation, TransparentPlaneMaterialXY, InnerRadius, OuterRadius);
		FColor OuterColor = (CurrentAxis&InAxis ? CurrentColor : InColor);
		//Pass through alpha
		OuterColor.A = InColor.A;
		DrawThickArc(OuterArcParams, Axis0, Axis1, InStartAngle, InEndAngle, OuterColor);
	}
	PDI->SetHitProxy(NULL);

	const bool bIsHitProxyView = View->Family->EngineShowFlags.HitProxies;
	if (bIsPerspective && !bIsHitProxyView && !PDI->IsHitTesting())
	{
		FThickArcParams InnerArcParams(PDI, InLocation, GridMaterial, 0.0f, InnerRadius);
		FColor InnerColor = InColor;
		//if something is selected and it's not this
		InnerColor.A = ((CurrentAxis & InAxis) && !MyViewportClient->bDragging) ? LargeInnerAlpha : SmallInnerAlpha;
		DrawThickArc(InnerArcParams, Axis0, Axis1, InStartAngle, InEndAngle, InnerColor);
	}
}

void FMyWidget::DrawStartStopMarker(FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const float InAngle, const FColor& InColor, const float InScale)
{
	const float ArrowHeightPercent = .8f;
	const float InnerDistance = INNER_AXIS_CIRCLE_RADIUS * InScale;
	const float OuterDistance = OUTER_AXIS_CIRCLE_RADIUS * InScale;
	const float RingHeight = OuterDistance - InnerDistance;
	const float ArrowHeight = RingHeight * ArrowHeightPercent;
	const float ThirtyDegrees = PI / 6.0f;
	const float HalfArrowidth = ArrowHeight * FMath::Tan(ThirtyDegrees);

	FVector ZAxis = Axis0 ^ Axis1;
	FVector RotatedAxis0 = Axis0.RotateAngleAxis(InAngle, ZAxis);
	FVector RotatedAxis1 = Axis1.RotateAngleAxis(InAngle, ZAxis);

	FVector Vertices[3];
	Vertices[0] = InLocation + (OuterDistance)*RotatedAxis0;
	Vertices[1] = Vertices[0] + (ArrowHeight)*RotatedAxis0 - HalfArrowidth * RotatedAxis1;
	Vertices[2] = Vertices[1] + (2 * HalfArrowidth)*RotatedAxis1;

	PDI->DrawLine(Vertices[0], Vertices[1], InColor, SDPG_Foreground);
	PDI->DrawLine(Vertices[1], Vertices[2], InColor, SDPG_Foreground);
	PDI->DrawLine(Vertices[0], Vertices[2], InColor, SDPG_Foreground);

	if (InColor.A > 0)
	{
		//fill in the box
		FDynamicMeshBuilder MeshBuilder(PDI->View->GetFeatureLevel());

		for (int32 VertexIndex = 0; VertexIndex < 3; VertexIndex++)
		{
			FDynamicMeshVertex MeshVertex;
			MeshVertex.Position = Vertices[VertexIndex];
			MeshVertex.Color = InColor;
			MeshVertex.TextureCoordinate[0] = FVector2D(0.0f, 0.0f);
			MeshVertex.SetTangents(
				RotatedAxis0,
				RotatedAxis1,
				(RotatedAxis0) ^ RotatedAxis1
			);
			MeshBuilder.AddVertex(MeshVertex); //Add bottom vertex
		}

		MeshBuilder.AddTriangle(0, 1, 2);
		MeshBuilder.Draw(PDI, FMatrix::Identity, TransparentPlaneMaterialXY->GetRenderProxy(false), SDPG_Foreground, 0.f);
	}
}

void FMyWidget::DrawSnapMarker(FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const FColor& InColor, const float InScale, const float InWidthPercent, const float InPercentSize)
{
	const float InnerDistance = INNER_AXIS_CIRCLE_RADIUS * InScale;
	const float OuterDistance = OUTER_AXIS_CIRCLE_RADIUS * InScale;
	const float MaxMarkerHeight = OuterDistance - InnerDistance;
	const float MarkerWidth = MaxMarkerHeight * InWidthPercent;
	const float MarkerHeight = MaxMarkerHeight * InPercentSize;

	FVector Vertices[4];
	Vertices[0] = InLocation + (OuterDistance)*Axis0 - (MarkerWidth*.5)*Axis1;
	Vertices[1] = Vertices[0] + (MarkerWidth)*Axis1;
	Vertices[2] = InLocation + (OuterDistance - MarkerHeight)*Axis0 - (MarkerWidth*.5)*Axis1;
	Vertices[3] = Vertices[2] + (MarkerWidth)*Axis1;

	//draw at least one line
	PDI->DrawLine(Vertices[0], Vertices[2], InColor, SDPG_Foreground);

	//if there should be thickness, draw the other lines
	if (InWidthPercent > 0.0f)
	{
		PDI->DrawLine(Vertices[0], Vertices[1], InColor, SDPG_Foreground);
		PDI->DrawLine(Vertices[1], Vertices[3], InColor, SDPG_Foreground);
		PDI->DrawLine(Vertices[2], Vertices[3], InColor, SDPG_Foreground);

		//fill in the box
		FDynamicMeshBuilder MeshBuilder(PDI->View->GetFeatureLevel());

		for (int32 VertexIndex = 0; VertexIndex < 4; VertexIndex++)
		{
			FDynamicMeshVertex MeshVertex;
			MeshVertex.Position = Vertices[VertexIndex];
			MeshVertex.Color = InColor;
			MeshVertex.TextureCoordinate[0] = FVector2D(0.0f, 0.0f);
			MeshVertex.SetTangents(
				Axis0,
				Axis1,
				(Axis0) ^ Axis1
			);
			MeshBuilder.AddVertex(MeshVertex); //Add bottom vertex
		}

		MeshBuilder.AddTriangle(0, 1, 2);
		MeshBuilder.AddTriangle(1, 3, 2);
		MeshBuilder.Draw(PDI, FMatrix::Identity, TransparentPlaneMaterialXY->GetRenderProxy(false), SDPG_Foreground, 0.f);
	}
}

void FMyWidget::DrawRotationArc(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const FVector& InDirectionToWidget, const FColor& InColor, const float InScale, FVector2D& OutAxisDir)
{
	bool bIsPerspective = (View->ViewMatrices.GetProjectionMatrix().M[3][3] < 1.0f);

	FColor ArcColor = InColor;
	ArcColor.A = LargeOuterAlpha;

	if (MyViewportClient->bDragging)
	{
		if (CurrentAxis&InAxis)
		{
			float DeltaRotation = TotalDeltaRotation;
			float AdjustedDeltaRotation = CustomCoordSystemSpace == COORD_Local ? -DeltaRotation : DeltaRotation;
			float AbsRotation = FRotator::ClampAxis(FMath::Abs(DeltaRotation));
			float AngleOfChangeRadians(AbsRotation * PI / 180.f);

			//always draw clockwise, so if we're negative we need to flip the angle
			float StartAngle = AdjustedDeltaRotation < 0.0f ? -AngleOfChangeRadians : 0.0f;
			float FilledAngle = AngleOfChangeRadians;

			//the axis of rotation
			FVector ZAxis = Axis0 ^ Axis1;

			ArcColor.A = LargeOuterAlpha;
			DrawPartialRotationArc(View, PDI, InAxis, InLocation, Axis0, Axis1, StartAngle, StartAngle + FilledAngle, ArcColor, InScale);
			ArcColor.A = SmallOuterAlpha;
			DrawPartialRotationArc(View, PDI, InAxis, InLocation, Axis0, Axis1, StartAngle + FilledAngle, StartAngle + 2 * PI, ArcColor, InScale);

			ArcColor = (CurrentAxis&InAxis) ? CurrentColor : ArcColor;
			//Hallow Arrow
			ArcColor.A = 0;
			DrawStartStopMarker(PDI, InLocation, Axis0, Axis1, 0, ArcColor, InScale);
			//Filled Arrow
			ArcColor.A = LargeOuterAlpha;
			DrawStartStopMarker(PDI, InLocation, Axis0, Axis1, AdjustedDeltaRotation, ArcColor, InScale);

			ArcColor.A = 255;

			FVector SnapLocation = InLocation;

			//if (GetDefault<ULevelEditorViewportSettings>()->RotGridEnabled)
			//{
			//	float DeltaAngle = GEditor->GetRotGridSize().Yaw;
			//	//every 22.5 degrees
			//	float TickMarker = 22.5f;
			//	for (float Angle = 0; Angle < 360.f; Angle += DeltaAngle)
			//	{
			//		FVector GridAxis = Axis0.RotateAngleAxis(Angle, ZAxis);
			//		float PercentSize = (FMath::Fmod(Angle, TickMarker) == 0) ? .75f : .25f;
			//		if (FMath::Fmod(Angle, 90.f) != 0)
			//		{
			//			DrawSnapMarker(PDI, SnapLocation, GridAxis, FVector::ZeroVector, ArcColor, InScale, 0.0f, PercentSize);
			//		}
			//	}
			//}

			//draw axis tick marks
			FColor AxisColor = InColor;
			//Rotate Colors to match Axis 0
			Swap(AxisColor.R, AxisColor.G);
			Swap(AxisColor.B, AxisColor.R);
			AxisColor.A = (AdjustedDeltaRotation == 0) ? MAX_uint8 : LargeOuterAlpha;
			DrawSnapMarker(PDI, SnapLocation, Axis0, Axis1, AxisColor, InScale, .25f);
			AxisColor.A = (AdjustedDeltaRotation == 180.f) ? MAX_uint8 : LargeOuterAlpha;
			DrawSnapMarker(PDI, SnapLocation, -Axis0, -Axis1, AxisColor, InScale, .25f);

			//Rotate Colors to match Axis 1
			Swap(AxisColor.R, AxisColor.G);
			Swap(AxisColor.B, AxisColor.R);
			AxisColor.A = (AdjustedDeltaRotation == 90.f) ? MAX_uint8 : LargeOuterAlpha;
			DrawSnapMarker(PDI, SnapLocation, Axis1, -Axis0, AxisColor, InScale, .25f);
			AxisColor.A = (AdjustedDeltaRotation == 270.f) ? MAX_uint8 : LargeOuterAlpha;
			DrawSnapMarker(PDI, SnapLocation, -Axis1, Axis0, AxisColor, InScale, .25f);

			if (MyViewportClient->bDragging)
			{
				float OffsetAngle = CustomCoordSystemSpace == COORD_Local ? 0 : AdjustedDeltaRotation;

				CacheRotationHUDText(View, PDI, InLocation, Axis0.RotateAngleAxis(OffsetAngle, ZAxis), Axis1.RotateAngleAxis(OffsetAngle, ZAxis), DeltaRotation, InScale);
			}
		}
	}
	else
	{
		//Reverse the axes based on camera view
		bool bMirrorAxis0 = ((Axis0 | InDirectionToWidget) <= 0.0f);
		bool bMirrorAxis1 = ((Axis1 | InDirectionToWidget) <= 0.0f);

		FVector RenderAxis0 = bMirrorAxis0 ? Axis0 : -Axis0;
		FVector RenderAxis1 = bMirrorAxis1 ? Axis1 : -Axis1;
		float Direction = (bMirrorAxis0 ^ bMirrorAxis1) ? -1.0f : 1.0f;

		DrawPartialRotationArc(View, PDI, InAxis, InLocation, RenderAxis0, RenderAxis1, 0, PI / 2, ArcColor, InScale);

		FVector2D Axis0ScreenLocation;
		if (!View->ScreenToPixel(View->WorldToScreen(InLocation + RenderAxis0 * 64.0f), Axis0ScreenLocation))
		{
			Axis0ScreenLocation.X = Axis0ScreenLocation.Y = 0;
		}

		FVector2D Axis1ScreenLocation;
		if (!View->ScreenToPixel(View->WorldToScreen(InLocation + RenderAxis1 * 64.0f), Axis1ScreenLocation))
		{
			Axis1ScreenLocation.X = Axis1ScreenLocation.Y = 0;
		}

		OutAxisDir = ((Axis1ScreenLocation - Axis0ScreenLocation) * Direction).GetSafeNormal();
	}
}

void FMyWidget::Render_Rotate(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget)
{
	float Scale = View->WorldToScreen(InLocation).W * (4.0f / View->UnscaledViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);

	//get the axes 
	FVector XAxis = CustomCoordSystem.TransformVector(FVector(1, 0, 0));
	FVector YAxis = CustomCoordSystem.TransformVector(FVector(0, 1, 0));
	FVector ZAxis = CustomCoordSystem.TransformVector(FVector(0, 0, 1));

	EAxisList::Type DrawAxis = EAxisList::Type::XYZ;

	FVector DirectionToWidget = View->IsPerspectiveProjection() ? (InLocation - View->ViewMatrices.GetViewOrigin()) : -View->GetViewDirection();
	DirectionToWidget.Normalize();

	// Draw a circle for each axis
	if (bDrawWidget)
	{
		//no draw the arc segments
		if (DrawAxis&EAxisList::X)
		{
			DrawRotationArc(View, PDI, EAxisList::X, InLocation, ZAxis, YAxis, DirectionToWidget, AxisColorX.ToFColor(true), Scale, XAxisDir);
		}

		if (DrawAxis&EAxisList::Y)
		{
			DrawRotationArc(View, PDI, EAxisList::Y, InLocation, XAxis, ZAxis, DirectionToWidget, AxisColorY.ToFColor(true), Scale, YAxisDir);
		}

		if (DrawAxis&EAxisList::Z)
		{
			DrawRotationArc(View, PDI, EAxisList::Z, InLocation, XAxis, YAxis, DirectionToWidget, AxisColorZ.ToFColor(true), Scale, ZAxisDir);
		}
	}
}

void FMyWidget::CacheRotationHUDText(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const float AngleOfChange, const float InScale)
{
	const float TextDistance = ROTATION_TEXT_RADIUS * InScale;

	FVector AxisVectors[4] = { Axis0, Axis1, -Axis0, -Axis1 };

	for (int i = 0; i < 4; ++i)
	{
		FVector PotentialTextPosition = InLocation + (TextDistance)*AxisVectors[i];
		if (View->ScreenToPixel(View->WorldToScreen(PotentialTextPosition), HUDInfoPos))
		{
			if (FMath::IsWithin<float>(HUDInfoPos.X, 0, View->UnscaledViewRect.Width()) && FMath::IsWithin<float>(HUDInfoPos.Y, 0, View->UnscaledViewRect.Height()))
			{
				//only valid screen locations get a valid string
				HUDString = FString::Printf(TEXT("%3.2f"), AngleOfChange);
				break;
			}
		}
	}
}

void FMyWidget::DrawHUD(FCanvas* Canvas)
{
	if (HUDString.Len())
	{
		const float DPIScale = Canvas->GetDPIScale();
		int32 StringPosX = FMath::FloorToInt(HUDInfoPos.X / DPIScale);
		int32 StringPosY = FMath::FloorToInt(HUDInfoPos.Y / DPIScale);

		//measure string size
		int32 StringSizeX, StringSizeY;
		StringSize(GEngine->GetSmallFont(), StringSizeX, StringSizeY, *HUDString);

		//add some padding to the outside
		const int32 Border = 5;
		int32 FillMinX = StringPosX - Border - (StringSizeX >> 1);
		int32 FillMinY = StringPosY - Border;// - (StringSizeY>>1);
		StringSizeX += 2 * Border;
		StringSizeY += 2 * Border;

		//mostly alpha'ed black
		FCanvasTileItem TileItem(FVector2D(FillMinX, FillMinY), GWhiteTexture, FVector2D(StringSizeX, StringSizeY), FLinearColor(0.0f, 0.0f, 0.0f, .25f));
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
		FCanvasTextItem TextItem(FVector2D(StringPosX, StringPosY), FText::FromString(HUDString), GEngine->GetSmallFont(), FLinearColor::White);
		TextItem.bCentreX = true;
		Canvas->DrawItem(TextItem);
	}
}

void FMyWidget::Render_Scale(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget)
{
	// Figure out axis colors
	const FLinearColor& XColor = (CurrentAxis&EAxisList::X ? (FLinearColor)CurrentColor : AxisColorX);
	const FLinearColor& YColor = (CurrentAxis&EAxisList::Y ? (FLinearColor)CurrentColor : AxisColorY);
	const FLinearColor& ZColor = (CurrentAxis&EAxisList::Z ? (FLinearColor)CurrentColor : AxisColorZ);

	// Figure out axis materials
	UMaterialInterface* XMaterial = (CurrentAxis&EAxisList::X ? CurrentAxisMaterial : AxisMaterialX);
	UMaterialInterface* YMaterial = (CurrentAxis&EAxisList::Y ? CurrentAxisMaterial : AxisMaterialY);
	UMaterialInterface* ZMaterial = (CurrentAxis&EAxisList::Z ? CurrentAxisMaterial : AxisMaterialZ);
	UMaterialInterface* XYZMaterial = (CurrentAxis&EAxisList::XYZ ? CurrentAxisMaterial : OpaquePlaneMaterialXY);

	FMatrix WidgetMatrix = CustomCoordSystem * FTranslationMatrix(InLocation);
	const EAxisList::Type DrawAxis = EAxisList::Type::XYZ;

	float MyUniformScale = View->WorldToScreen(InLocation).W * (4.0f / View->UnscaledViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);
	const FVector UniformScale(MyUniformScale);

	Render_Axis(View, PDI, EAxisList::X, WidgetMatrix, XMaterial, XColor, XAxisDir, UniformScale, bDrawWidget, true);

	Render_Axis(View, PDI, EAxisList::Y, WidgetMatrix, YMaterial, YColor, YAxisDir, UniformScale, bDrawWidget, true);

	Render_Axis(View, PDI, EAxisList::Z, WidgetMatrix, ZMaterial, ZColor, ZAxisDir, UniformScale, bDrawWidget, true);

	// Draw grabber handles and center cube
	if (bDrawWidget)
	{
		const bool bDisabled = false;

		// Grabber handles - since orthographic scale widgets are not supported, we should always draw grabber handles if we're drawing
		// the corresponding axes.
		if ((DrawAxis&(EAxisList::X | EAxisList::Y)) == (EAxisList::X | EAxisList::Y))
		{
			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::XY, bDisabled));
			{
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(24, 0, 0) * UniformScale), WidgetMatrix.TransformPosition(FVector(12, 12, 0) * UniformScale), XColor, SDPG_Foreground);
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(12, 12, 0) * UniformScale), WidgetMatrix.TransformPosition(FVector(0, 24, 0) * UniformScale), YColor, SDPG_Foreground);
			}
			PDI->SetHitProxy(NULL);
		}

		if ((DrawAxis&(EAxisList::X | EAxisList::Z)) == (EAxisList::X | EAxisList::Z))
		{
			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::XZ, bDisabled));
			{
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(24, 0, 0) * UniformScale), WidgetMatrix.TransformPosition(FVector(12, 0, 12) * UniformScale), XColor, SDPG_Foreground);
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(12, 0, 12) * UniformScale), WidgetMatrix.TransformPosition(FVector(0, 0, 24) * UniformScale), ZColor, SDPG_Foreground);
			}
			PDI->SetHitProxy(NULL);
		}

		if ((DrawAxis&(EAxisList::Y | EAxisList::Z)) == (EAxisList::Y | EAxisList::Z))
		{
			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::YZ, bDisabled));
			{
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(0, 24, 0) * UniformScale), WidgetMatrix.TransformPosition(FVector(0, 12, 12) * UniformScale), YColor, SDPG_Foreground);
				PDI->DrawLine(WidgetMatrix.TransformPosition(FVector(0, 12, 12) * UniformScale), WidgetMatrix.TransformPosition(FVector(0, 0, 24) * UniformScale), ZColor, SDPG_Foreground);
			}
			PDI->SetHitProxy(NULL);
		}

		// Center cube
		if ((DrawAxis&(EAxisList::XYZ)) == EAxisList::XYZ)
		{
			PDI->SetHitProxy(new HMyWidgetAxis(EAxisList::XYZ, bDisabled));

			Render_Cube(PDI, WidgetMatrix, XYZMaterial, UniformScale * 4);

			PDI->SetHitProxy(NULL);
		}
	}
}

void GetAxisPlaneNormalAndMask(const FMatrix& InCoordSystem, const FVector& InAxis, const FVector& InDirToPixel, FVector& OutPlaneNormal, FVector& NormalToRemove)
{
	FVector XAxis = InCoordSystem.TransformVector(FVector(1, 0, 0));
	FVector YAxis = InCoordSystem.TransformVector(FVector(0, 1, 0));
	FVector ZAxis = InCoordSystem.TransformVector(FVector(0, 0, 1));

	float XDot = FMath::Abs(InDirToPixel | XAxis);
	float YDot = FMath::Abs(InDirToPixel | YAxis);
	float ZDot = FMath::Abs(InDirToPixel | ZAxis);

	if ((InAxis | XAxis) > .1f)
	{
		OutPlaneNormal = (YDot > ZDot) ? YAxis : ZAxis;
		NormalToRemove = (YDot > ZDot) ? ZAxis : YAxis;
	}
	else if ((InAxis | YAxis) > .1f)
	{
		OutPlaneNormal = (XDot > ZDot) ? XAxis : ZAxis;
		NormalToRemove = (XDot > ZDot) ? ZAxis : XAxis;
	}
	else
	{
		OutPlaneNormal = (XDot > YDot) ? XAxis : YAxis;
		NormalToRemove = (XDot > YDot) ? YAxis : XAxis;
	}
}

void GetPlaneNormalAndMask(const FVector& InAxis, FVector& OutPlaneNormal, FVector& NormalToRemove)
{
	OutPlaneNormal = InAxis;
	NormalToRemove = InAxis;
}

void FMyWidget::AbsoluteTranslationConvertMouseMovementToAxisMovement(FSceneView* InView, const FVector& InLocation, const FVector2D& InMousePosition, FVector& OutDrag)
{
	// Compute a world space ray from the screen space mouse coordinates
	FViewportCursorLocation MouseViewportRay(InView, InMousePosition.X, InMousePosition.Y);

	FAbsoluteMovementParams Params;
	Params.EyePos = MouseViewportRay.GetOrigin();
	Params.PixelDir = MouseViewportRay.GetDirection();
	Params.CameraDir = InView->GetViewDirection();
	Params.Position = InLocation;
	//dampen by 
	Params.bMovementLockedToCamera = false;
	Params.bPositionSnapping = false;

	FMatrix InputCoordSystem = FMatrix::Identity;
	if (CustomCoordSystemSpace == COORD_Local && MyViewportClient->SelectedActor)
	{
		InputCoordSystem = FQuatRotationMatrix(MyViewportClient->SelectedActor->GetActorQuat());
		if (!InputCoordSystem.Equals(FMatrix::Identity))
		{
			InputCoordSystem.RemoveScaling();
		}
	}

	Params.XAxis = InputCoordSystem.TransformVector(FVector(1, 0, 0));
	Params.YAxis = InputCoordSystem.TransformVector(FVector(0, 1, 0));
	Params.ZAxis = InputCoordSystem.TransformVector(FVector(0, 0, 1));

	switch (CurrentAxis)
	{
	case EAxisList::X:  GetAxisPlaneNormalAndMask(InputCoordSystem, Params.XAxis, Params.CameraDir, Params.PlaneNormal, Params.NormalToRemove); break;
	case EAxisList::Y:  GetAxisPlaneNormalAndMask(InputCoordSystem, Params.YAxis, Params.CameraDir, Params.PlaneNormal, Params.NormalToRemove); break;
	case EAxisList::Z:  GetAxisPlaneNormalAndMask(InputCoordSystem, Params.ZAxis, Params.CameraDir, Params.PlaneNormal, Params.NormalToRemove); break;
	case EAxisList::XY: GetPlaneNormalAndMask(Params.ZAxis, Params.PlaneNormal, Params.NormalToRemove); break;
	case EAxisList::XZ: GetPlaneNormalAndMask(Params.YAxis, Params.PlaneNormal, Params.NormalToRemove); break;
	case EAxisList::YZ: GetPlaneNormalAndMask(Params.XAxis, Params.PlaneNormal, Params.NormalToRemove); break;
	}

	OutDrag = GetAbsoluteTranslationDelta(Params);
}

#define CAMERA_LOCK_DAMPING_FACTOR .1f
#define MAX_CAMERA_MOVEMENT_SPEED 512.0f

FVector FMyWidget::GetAbsoluteTranslationDelta(const FAbsoluteMovementParams& InParams)
{
	FPlane MovementPlane(InParams.Position, InParams.PlaneNormal);
	FVector ProposedEndofEyeVector = InParams.EyePos + (InParams.PixelDir * (InParams.Position - InParams.EyePos).Size());

	//default to not moving
	FVector RequestedPosition = InParams.Position;

	float DotProductWithPlaneNormal = InParams.PixelDir | InParams.PlaneNormal;
	//check to make sure we're not co-planar
	if (FMath::Abs(DotProductWithPlaneNormal) > DELTA)
	{
		//Get closest point on plane
		RequestedPosition = FMath::LinePlaneIntersection(InParams.EyePos, ProposedEndofEyeVector, MovementPlane);
	}

	//drag is a delta position, so just update the different between the previous position and the new position
	FVector DeltaPosition = RequestedPosition - InParams.Position;

	//Retrieve the initial offset, passing in the current requested position and the current position
	FVector InitialOffset = GetAbsoluteTranslationInitialOffset(RequestedPosition, InParams.Position);

	//subtract off the initial offset (where the widget was clicked) to prevent popping
	DeltaPosition -= InitialOffset;

	//remove the component along the normal we want to mute
	float MovementAlongMutedAxis = DeltaPosition | InParams.NormalToRemove;
	FVector OutDrag = DeltaPosition - (InParams.NormalToRemove*MovementAlongMutedAxis);

	if (InParams.bMovementLockedToCamera)
	{
		//DAMPEN ABSOLUTE MOVEMENT when the camera is locked to the object
		OutDrag *= CAMERA_LOCK_DAMPING_FACTOR;
		OutDrag.X = FMath::Clamp(OutDrag.X, -MAX_CAMERA_MOVEMENT_SPEED, MAX_CAMERA_MOVEMENT_SPEED);
		OutDrag.Y = FMath::Clamp(OutDrag.Y, -MAX_CAMERA_MOVEMENT_SPEED, MAX_CAMERA_MOVEMENT_SPEED);
		OutDrag.Z = FMath::Clamp(OutDrag.Z, -MAX_CAMERA_MOVEMENT_SPEED, MAX_CAMERA_MOVEMENT_SPEED);
	}

	//get the distance from the original position to the new proposed position 
	FVector DeltaFromStart = InParams.Position + OutDrag - InitialTranslationPosition;

	//Get the vector from the eye to the proposed new position (to make sure it's not behind the camera
	FVector EyeToNewPosition = (InParams.Position + OutDrag) - InParams.EyePos;
	float BehindTheCameraDotProduct = EyeToNewPosition | InParams.CameraDir;

	//Don't let the requested position go behind the camera
	if (BehindTheCameraDotProduct <= 0)
	{
		OutDrag = OutDrag.ZeroVector;
	}
	return OutDrag;
}

FVector FMyWidget::GetAbsoluteTranslationInitialOffset(const FVector& InNewPosition, const FVector& InCurrentPosition)
{
	if (!bAbsoluteTranslationInitialOffsetCached)
	{
		bAbsoluteTranslationInitialOffsetCached = true;
		InitialTranslationOffset = InNewPosition - InCurrentPosition;
		InitialTranslationPosition = InCurrentPosition;
	}
	return InitialTranslationOffset;
}

void FMyWidget::ConvertMouseMovementToAxisMovement(FVector& InOutDelta, FRotator& OutRotation, FVector& OutScale)
{
	OutRotation = FRotator::ZeroRotator;
	OutScale = FVector::ZeroVector;

	// Get input delta as 2D vector, adjusted for inverted screen space Y axis
	const FVector2D DragDir = FVector2D(InOutDelta.X, -InOutDelta.Y);

	switch (MyViewportClient->WidgetMode)
	{
	case WM_Rotate:
	{
		FRotator Rotation;
		FVector2D EffectiveDelta;

		if (CurrentAxis == EAxisList::X)
		{
			// Get screen direction representing positive rotation
			const FVector2D AxisDir = XAxisDir;

			// Get rotation in widget local space
			Rotation = FRotator(0, 0, FVector2D::DotProduct(AxisDir, DragDir));

			// Record delta rotation (used by the widget to render the accumulated delta)
			CurrentDeltaRotation = Rotation.Roll;

			// Use to calculate the new input delta
			EffectiveDelta = AxisDir * Rotation.Roll;
		}
		else if (CurrentAxis == EAxisList::Y)
		{
			// TODO: Determine why -TangentDir is necessary here, and fix whatever is causing it
			const FVector2D AxisDir = YAxisDir;

			Rotation = FRotator(FVector2D::DotProduct(AxisDir, DragDir), 0, 0);

			CurrentDeltaRotation = Rotation.Pitch;
			EffectiveDelta = AxisDir * Rotation.Pitch;
		}
		else if (CurrentAxis == EAxisList::Z)
		{
			const FVector2D AxisDir = ZAxisDir;

			Rotation = FRotator(0, FVector2D::DotProduct(AxisDir, DragDir), 0);

			CurrentDeltaRotation = Rotation.Yaw;
			EffectiveDelta = AxisDir * Rotation.Yaw;
		}

		// Adjust the input delta according to how much rotation was actually applied
		InOutDelta = FVector(EffectiveDelta.X, -EffectiveDelta.Y, 0.0f);

		// Need to get the delta rotation in the current coordinate space of the widget
		OutRotation = (CustomCoordSystem.Inverse() * FRotationMatrix(Rotation) * CustomCoordSystem).Rotator();
	}
	break;

	case WM_Scale:
	{
		FVector2D AxisDir = FVector2D::ZeroVector;
		const float ScaleSpeedMultipler = 0.01f;

		if (CurrentAxis & EAxisList::X)
		{
			AxisDir += XAxisDir;
		}

		if (CurrentAxis & EAxisList::Y)
		{
			AxisDir += YAxisDir;
		}

		if (CurrentAxis & EAxisList::Z)
		{
			AxisDir += ZAxisDir;
		}

		AxisDir.Normalize();
		const float ScaleDelta = FVector2D::DotProduct(AxisDir, DragDir) * ScaleSpeedMultipler;

		OutScale = FVector(
			(CurrentAxis & EAxisList::X) ? ScaleDelta : 0.0f,
			(CurrentAxis & EAxisList::Y) ? ScaleDelta : 0.0f,
			(CurrentAxis & EAxisList::Z) ? ScaleDelta : 0.0f
		);

		// Convert to effective screen space delta, and replace input delta, adjusted for inverted screen space Y axis
		const float ScaleMax = OutScale.GetMax();
		const float ScaleMin = OutScale.GetMin();
		const float ScaleApplied = (ScaleMax > -ScaleMin) ? ScaleMax : ScaleMin;
		const FVector2D EffectiveDelta = AxisDir * ScaleApplied;
		InOutDelta = FVector(EffectiveDelta.X, -EffectiveDelta.Y, 0.0f);
	}
	break;

	default:
		break;
	}
}

void FMyWidget::UpdateDeltaRotation()
{
	TotalDeltaRotation += CurrentDeltaRotation;
	if ((TotalDeltaRotation <= -360.f) || (TotalDeltaRotation >= 360.f))
	{
		TotalDeltaRotation = FRotator::ClampAxis(TotalDeltaRotation);
	}
}