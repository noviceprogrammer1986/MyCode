// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPointActor.generated.h"


class UTexture2D;
class UMaterialInstanceDynamic;
class USceneComponent;

UCLASS()
class TCP_API APatrolPointActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPointActor();
	
	UFUNCTION()
		void SetTexture(UTexture2D* InTexture);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int32 PointNum = 0;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ImageMesh;
	UPROPERTY()
		USceneComponent* Root;
	UPROPERTY()
		UMaterialInstanceDynamic* MID;
};
