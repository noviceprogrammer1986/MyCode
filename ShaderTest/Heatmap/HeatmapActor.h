// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeatmapActor.generated.h"


USTRUCT()
struct FMyShaderParameters
{
	GENERATED_BODY()
public:
	UPROPERTY()
		float a;
	UPROPERTY()
		float b;
};

class UTextureRenderTarget2D;

UCLASS()
class SHADERTEST_API AHeatmapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeatmapActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
		UTextureRenderTarget2D* RenderTarget;

public:	
	void ExecuteComputerShader_RenderThread(UTextureRenderTarget2D* RenderTarget, FMyShaderParameters MyShaderParameters);

};
