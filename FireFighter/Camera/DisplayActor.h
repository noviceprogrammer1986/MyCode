// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DisplayActor.generated.h"

UCLASS()
class FIREFIGHTER_API ADisplayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisplayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetMeshMat(UMaterialInstanceDynamic* MID, float ScaleX);

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere)
		class UParticleSystemComponent* Particle;

	UPROPERTY(VisibleAnywhere)
		class UMaterialBillboardComponent* Billboard;

private:
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(NetMulticast, Reliable)
		void OnOverlap_Multicast();
};
