// Fill out your copyright notice in the Description page of Project Settings.

#include "ParticleActor.h"
#include "Components/MaterialBillboardComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"


// Sets default values
AParticleActor::AParticleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Comp = CreateDefaultSubobject<UMaterialBillboardComponent>("Comp");
	SetRootComponent(Comp);
}

// Called when the game starts or when spawned
void AParticleActor::BeginPlay()
{
	Super::BeginPlay();

	Speed = FMath::FRandRange(SpeedRange.X, SpeedRange.Y);
	Length = FMath::FRandRange(LengthRange.X, LengthRange.Y);
	Lifetime = FMath::FRandRange(LifetimeRange.X, LifetimeRange.Y);
	FadeOutTime = FMath::FRandRange(FadeOutTimeRange.X, FadeOutTimeRange.Y);
	if (ParticleMaterial)
	{
		MID = UMaterialInstanceDynamic::Create(ParticleMaterial, this);
		if (MID && Comp->Elements.Num() > 0)
		{
			Comp->Elements[0].Material = MID;
			MID->SetScalarParameterValue("Length", Length);
		}
	}
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [&] { Destroy(); }, Lifetime, false);
}

// Called every frame
void AParticleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MID)
	{
		Top += DeltaTime * Speed;
		MID->SetScalarParameterValue("Top", Top);

		Lifetime -= DeltaTime;
		if (Lifetime < FadeOutTime)
		{
			MID->SetScalarParameterValue("Fade", Lifetime / FadeOutTime);
		}

		Comp->MarkRenderStateDirty();
	}
}

