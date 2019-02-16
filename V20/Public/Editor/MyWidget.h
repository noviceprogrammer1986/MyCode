// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneManagement.h"


class UMyGameViewportClient;

enum ECoordSystem
{
	COORD_None = -1,
	COORD_World,
	COORD_Local,
	COORD_Max,
};

enum EWidgetMode
{
	WM_None = -1,
	WM_Translate,
	WM_Rotate,
	WM_Scale,
	WM_Max,
};

class V20_API FMyWidget : public FGCObject
{
public:
	FMyWidget(UMyGameViewportClient* MyViewportClient);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	void Render(const FSceneView* View, FPrimitiveDrawInterface* PDI);

	void AbsoluteTranslationConvertMouseMovementToAxisMovement(FSceneView* InView, const FVector& InLocation, const FVector2D& InMousePosition,
		FVector& OutDrag);

	void ConvertMouseMovementToAxisMovement(FVector& InDiff, FRotator& OutRotation, FVector& OutScale);

	void UpdateDeltaRotation();
	/** Resets the total delta rotation back to zero */
	void ResetDeltaRotation() { TotalDeltaRotation = 0; }

private:
	struct FThickArcParams
	{
		FThickArcParams(FPrimitiveDrawInterface* InPDI, const FVector& InPosition, UMaterialInterface* InMaterial,
			const float InInnerRadius, const float InOuterRadius)
			: Position(InPosition)
			, PDI(InPDI)
			, Material(InMaterial)
			, InnerRadius(InInnerRadius)
			, OuterRadius(InOuterRadius)
		{
		}

		FVector Position;
		FPrimitiveDrawInterface* PDI;
		UMaterialInterface* Material;
		float InnerRadius;
		float OuterRadius;
	};

	struct FViewportCursorLocation
	{
	public:
		FViewportCursorLocation(const FSceneView* View, int32 X, int32 Y)
			: Origin(ForceInit), Direction(ForceInit), CursorPos(X, Y)
		{
			FVector4 ScreenPos = View->PixelToScreen(X, Y, 0);

			const FMatrix InvViewMatrix = View->ViewMatrices.GetInvViewMatrix();
			const FMatrix InvProjMatrix = View->ViewMatrices.GetInvProjectionMatrix();

			const float ScreenX = ScreenPos.X;
			const float ScreenY = ScreenPos.Y;

			Origin = View->ViewMatrices.GetViewOrigin();
			Direction = InvViewMatrix.TransformVector(FVector(InvProjMatrix.TransformFVector4(
				FVector4(ScreenX * GNearClippingPlane, ScreenY * GNearClippingPlane, 0.0f, GNearClippingPlane)))).GetSafeNormal();
		}

		const FVector&		GetOrigin()			const { return Origin; }
		const FVector&		GetDirection()		const { return Direction; }
		const FIntPoint&	GetCursorPos()		const { return CursorPos; }

	private:
		FVector	Origin;
		FVector	Direction;
		FIntPoint CursorPos;
	};

	struct FAbsoluteMovementParams
	{
		/** The normal of the plane to project onto */
		FVector PlaneNormal;
		/** A vector that represent any displacement we want to mute (remove an axis if we're doing axis movement)*/
		FVector NormalToRemove;
		/** The current position of the widget */
		FVector Position;

		//Coordinate System Axes
		FVector XAxis;
		FVector YAxis;
		FVector ZAxis;

		//true if camera movement is locked to the object
		bool bMovementLockedToCamera;

		//Direction in world space to the current mouse location
		FVector PixelDir;
		//Direction in world space of the middle of the camera
		FVector CameraDir;
		FVector EyePos;

		//whether to snap the requested positionto the grid
		bool bPositionSnapping;
	};

private:
	void DrawHUD(FCanvas* Canvas);

	void Render_Cube(FPrimitiveDrawInterface* PDI, const FMatrix& InMatrix, const UMaterialInterface* InMaterial, const FVector& InScale);

	void Render_Axis(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, FMatrix& InMatrix, UMaterialInterface* InMaterial,
		const FLinearColor& InColor, FVector2D& OutAxisDir, const FVector& InScale, bool bDrawWidget, bool bCubeHead = false);

	void Render_Translate(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget);

	void DrawThickArc(const FThickArcParams& InParams, const FVector& Axis0, const FVector& Axis1, const float InStartAngle, const float InEndAngle,
		const FColor& InColor);

	void DrawPartialRotationArc(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, const FVector& InLocation,
		const FVector& Axis0, const FVector& Axis1, const float InStartAngle, const float InEndAngle, const FColor& InColor, const float InScale);

	void DrawStartStopMarker(FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const float InAngle,
		const FColor& InColor, const float InScale);

	void DrawSnapMarker(FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0, const FVector& Axis1, const FColor& InColor,
		const float InScale, const float InWidthPercent = 0.0f, const float InPercentSize = 1.0f);

	void DrawRotationArc(const FSceneView* View, FPrimitiveDrawInterface* PDI, EAxisList::Type InAxis, const FVector& InLocation, const FVector& Axis0,
		const FVector& Axis1, const FVector& InDirectionToWidget, const FColor& InColor, const float InScale, FVector2D& OutAxisEnd);

	void Render_Rotate(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget);

	void Render_Scale(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, bool bDrawWidget);

	void CacheRotationHUDText(const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& InLocation, const FVector& Axis0,
		const FVector& Axis1, const float AngleOfChange, const float InScale);

	FVector GetAbsoluteTranslationDelta(const FAbsoluteMovementParams& InParams);
	FVector GetAbsoluteTranslationInitialOffset(const FVector& InNewPosition, const FVector& InCurrentPosition);

public:
	FVector WidgetLocation;
	EAxisList::Type CurrentAxis = EAxisList::None;
	bool bDrawWidget = false;
	/** Whether Absolute Translation cache position has been captured */
	bool bAbsoluteTranslationInitialOffsetCached = false;
	FMatrix CustomCoordSystem;
	ECoordSystem CustomCoordSystemSpace;

private:
	FLinearColor AxisColorX, AxisColorY, AxisColorZ;
	FColor CurrentColor;
	FVector2D XAxisDir, YAxisDir, ZAxisDir;

	UMaterialInterface* AxisMaterialBase;
	UMaterialInstanceDynamic* AxisMaterialX;
	UMaterialInstanceDynamic* AxisMaterialY;
	UMaterialInstanceDynamic* AxisMaterialZ;
	UMaterialInstanceDynamic* CurrentAxisMaterial;
	UMaterialInstanceDynamic* OpaquePlaneMaterialXY;
	UMaterialInterface* TransparentPlaneMaterialXY;
	UMaterialInterface* GridMaterial;

	/** Current delta rotation applied to the rotation widget */
	float CurrentDeltaRotation = 0.f;
	//location in the viewport to render the hud string
	FVector2D HUDInfoPos;
	//string to be displayed on top of the viewport
	FString HUDString;
	/** The initial offset where the widget was first clicked */
	FVector InitialTranslationOffset = FVector::ZeroVector;
	/** The initial position of the widget before it was clicked */
	FVector InitialTranslationPosition = FVector::ZeroVector;
	/** Total delta rotation applied since the widget was dragged */
	float TotalDeltaRotation;

	UMyGameViewportClient* MyViewportClient;
};

struct HMyWidgetAxis : public HHitProxy
{
	DECLARE_HIT_PROXY(V20_API);

	EAxisList::Type Axis;
	uint32 bDisabled : 1;

	HMyWidgetAxis(EAxisList::Type InAxis, bool InbDisabled = false) :
		HHitProxy(HPP_UI),
		Axis(InAxis),
		bDisabled(InbDisabled) {}

	virtual EMouseCursor::Type GetMouseCursor() override
	{
		if (bDisabled)
		{
			return EMouseCursor::Default;
		}
		return EMouseCursor::CardinalCross;
	}

	virtual bool AlwaysAllowsTranslucentPrimitives() const override
	{
		return true;
	}
};