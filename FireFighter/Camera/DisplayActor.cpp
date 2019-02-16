// Fill out your copyright notice in the Description page of Project Settings.

#include "DisplayActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "GameFramework/Pawn.h"


// Sets default values
ADisplayActor::ADisplayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	RootComponent = Box;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Particle = CreateDefaultSubobject<UParticleSystemComponent>("Particle");
	Particle->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Billboard = CreateDefaultSubobject<UMaterialBillboardComponent>("Billboard");
	Billboard->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ADisplayActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() < NM_Client)
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &ADisplayActor::OnOverlap);
	}
}

void ADisplayActor::SetMeshMat(UMaterialInstanceDynamic* MID, float ScaleX)
{
	Mesh->SetRelativeScale3D(Mesh->RelativeScale3D * FVector(ScaleX, 1.0, 1.0));
	Mesh->SetMaterial(0, MID);
}

// Called every frame
void ADisplayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADisplayActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(APawn::StaticClass()))
	{
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OnOverlap_Multicast();
	}
}

void ADisplayActor::OnOverlap_Multicast_Implementation()
{
	Particle->Activate(true);
	Billboard->SetHiddenInGame(true);
	Mesh->SetHiddenInGame(false);
}