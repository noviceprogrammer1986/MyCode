// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GizmoManager.generated.h"


class UMyGameViewportClient;
class UMyGameInstance;

UCLASS()
class V20_API AGizmoManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGizmoManager();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
		void EnterEditMode();
	UFUNCTION(BlueprintCallable)
		void ExitEditMode();
	UFUNCTION(BlueprintCallable)
		void SetWidgetMode(int32 Mode);
	UFUNCTION(BlueprintCallable)
		void SetSelectedActor(AActor* SelectedActor);
	UFUNCTION(BlueprintCallable)
		void ToggleCoordSystemSpace();
	UFUNCTION(BlueprintCallable)
		void ToggleWidgetMode();
	UFUNCTION(BlueprintCallable)
		void DestroySelectedActor();
	UFUNCTION(BlueprintCallable)
		AActor* GetSelectedActor() const;

private:
	UMyGameViewportClient* MyGameViewport;
	UMyGameInstance* MyGameInstance;
};
