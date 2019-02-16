// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "MyGameViewportClient.generated.h"


enum EWidgetMode;
class FMyWidget;
class UMyGameInstance;

DECLARE_MULTICAST_DELEGATE(FLocationChanged);
DECLARE_MULTICAST_DELEGATE(FRotationChanged);
DECLARE_MULTICAST_DELEGATE(FScaleChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FActorSelected, AActor*);

UCLASS()
class V20_API UMyGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed = 1.f, bool bGamepad = false) override;
	virtual void Tick(float DeltaTime) override;

	virtual void Draw(FViewport* Viewport, FCanvas* SceneCanvas) override;

	virtual bool RequiresHitProxyStorage() override { return true; }

	virtual EMouseCursor::Type GetCursor(FViewport* Viewport, int32 X, int32 Y) override;

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	virtual void BeginDestroy() override;

	EWidgetMode WidgetMode;
	bool bDragging = false;

	FMyWidget* MyWidget;
	bool bEditMode = false;
	bool bUseIcon = false;

	UPROPERTY()
		AActor* SelectedActor;
	FLocationChanged OnLocationChanged;
	FRotationChanged OnRotationChanged;
	FScaleChanged OnScaleChanged;
	FActorSelected OnActorSelected;

public:
	UFUNCTION()
		void EnterEditMode();
	UFUNCTION()
		void ExitEditMode();
	UFUNCTION()
		void SetWidgetMode(int32 Mode);
	UFUNCTION()
		void SetSelectedActor(AActor* InSelectedActor);
	UFUNCTION()
		void ToggleCoordSystemSpace();
	UFUNCTION()
		void ToggleWidgetMode();

private:
	/** Current buffer visualization mode for this game viewport */
	FName CurrentBufferVisualizationMode;
	/** Whether or not this audio device is in audio-focus */
	bool bHasAudioFocus;

	void CheckHoveredHitProxy(HHitProxy* HoveredHitProxy);

	bool bHovered = false;

	UPROPERTY()
		UMyGameInstance* MyGameInstance;
};