// Fill out your copyright notice in the Description page of Project Settings.

#include "PatrolPointActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
APatrolPointActor::APatrolPointActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	ImageMesh = CreateDefaultSubobject<UStaticMeshComponent>("ImageMesh");
	ImageMesh->SetupAttachment(RootComponent);
	ImageMesh->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void APatrolPointActor::BeginPlay()
{
	Super::BeginPlay();
	
	MID = ImageMesh->CreateDynamicMaterialInstance(0);
}

void APatrolPointActor::SetTexture(UTexture2D* InTexture)
{
	if (MID)
	{
		MID->SetTextureParameterValue("Param", InTexture);
		ImageMesh->SetHiddenInGame(false);
	}
}
