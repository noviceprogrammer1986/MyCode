// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleManager.generated.h"


class AParticleActor;

UCLASS()
class PARTICLELINE_API AParticleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParticleManager();

	UFUNCTION(BlueprintCallable)
		void StartSpawn();
	UFUNCTION(BlueprintCallable)
		void StopSpawn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere)
		TSubclassOf<AParticleActor> ParticleActorClass;
	UPROPERTY(EditAnywhere)
		float SpawnRate = 1.0;
	UPROPERTY(EditAnywhere)
		FVector SpawnBoxExtent = FVector(5000, 5000, 50);
	UPROPERTY(EditAnywhere)
		float MinDistance = 100.0;

private:
	FTimerHandle SpawnHandle;
	FBox SpawnBox;

	UPROPERTY()
		USceneComponent* Scene;
};
