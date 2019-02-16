// Fill out your copyright notice in the Description page of Project Settings.

#include "ParticleManager.h"
#include "TimerManager.h"
#include "ParticleActor.h"
#include "EngineUtils.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AParticleManager::AParticleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);
}

// Called when the game starts or when spawned
void AParticleManager::BeginPlay()
{
	Super::BeginPlay();

	//StartSpawn();
}

void AParticleManager::StartSpawn()
{
	const FVector BoxMin = GetActorLocation() - SpawnBoxExtent;
	const FVector BoxMax = GetActorLocation() + SpawnBoxExtent;
	SpawnBox = FBox(BoxMin, BoxMax);

	if (ParticleActorClass)
	{
		GetWorldTimerManager().SetTimer(SpawnHandle, [&]
		{
			FVector SpawnLocation = FMath::RandPointInBox(SpawnBox);
			FVector CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
			while (FVector::DistSquaredXY(SpawnLocation, CameraLocation) < MinDistance * MinDistance)
			{
				SpawnLocation = FMath::RandPointInBox(SpawnBox);
			}
			GetWorld()->SpawnActor(ParticleActorClass, &SpawnLocation);
		}, 1.f / SpawnRate, true, 0.f);
	}
}

void AParticleManager::StopSpawn()
{
	GetWorldTimerManager().ClearTimer(SpawnHandle);
	for (TActorIterator<AParticleActor> Itr(GetWorld()); Itr; ++Itr)
	{
		Itr->Destroy();
	}
}