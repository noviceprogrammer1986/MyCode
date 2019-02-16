// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleActor.generated.h"

UCLASS()
class PARTICLELINE_API AParticleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		class UMaterialBillboardComponent* Comp;
	UPROPERTY(EditAnywhere)
		class UMaterialInterface* ParticleMaterial;
	UPROPERTY(EditAnywhere)
		FVector2D LifetimeRange = FVector2D(10, 20);
	UPROPERTY(EditAnywhere)
		FVector2D SpeedRange = FVector2D(100, 200);
	UPROPERTY(EditAnywhere)
		FVector2D LengthRange = FVector2D(500, 2000);
	UPROPERTY(EditAnywhere)
		FVector2D FadeOutTimeRange = FVector2D(3, 5);

private:
	UPROPERTY()
		class UMaterialInstanceDynamic* MID;
	UPROPERTY()
		float Speed = 0;
	UPROPERTY()
		float Top = 0;
	UPROPERTY()
		float Length = 1000;
	UPROPERTY()
		float FadeOutTime = 0;
	UPROPERTY()
		float Lifetime = 0;
};
