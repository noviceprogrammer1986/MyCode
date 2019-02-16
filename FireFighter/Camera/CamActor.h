// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CamActor.generated.h"


UCLASS()
class FIREFIGHTER_API ACamActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACamActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//UPROPERTY(VisibleAnywhere)
		//UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
		UMaterial* CamMaterial;

	UPROPERTY(EditDefaultsOnly)
		FString CameraIP;

	UPROPERTY(EditDefaultsOnly)
		int32 CameraID;

private:
	class FAsyncCamReader* CamReader;

	UTexture2D* CamTexture;
	FTimerHandle Handle_CheckTexture;
	UMaterialInstanceDynamic* MID;
};
