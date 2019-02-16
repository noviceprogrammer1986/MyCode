// Fill out your copyright notice in the Description page of Project Settings.

#include "CamActor.h"
#include "OpenCV/Public/AsyncCamReader.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "EngineUtils.h"
#include "DisplayActor.h"


// Sets default values
ACamActor::ACamActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	//RootComponent = Mesh;
	CamMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
}

// Called when the game starts or when spawned
void ACamActor::BeginPlay()
{
	Super::BeginPlay();
	
	CamReader = new FAsyncCamReader("CamReader", CameraIP, CameraID);

	GetWorldTimerManager().SetTimer(Handle_CheckTexture, [&]
	{
		if (CamReader && CamReader->HasTexture)
		{
			GetWorldTimerManager().ClearTimer(Handle_CheckTexture);

			CamTexture = CamReader->VideoTexture;
			MID = UMaterialInstanceDynamic::Create(CamMaterial, this);
			MID->SetTextureParameterValue("Texture", CamTexture);
			//Mesh->SetMaterial(0, MID);
			//Mesh->SetRelativeScale3D(Mesh->RelativeScale3D * FVector(CamReader->VideoSize.X / CamReader->VideoSize.Y, 1.0, 1.0));
			for (TActorIterator<ADisplayActor>Itr(GetWorld()); Itr; ++Itr)
			{
				(*Itr)->SetMeshMat(MID, CamReader->VideoSize.X / CamReader->VideoSize.Y);
			}
		}
	}, 0.1, true);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [&] { GetWorldTimerManager().ClearTimer(Handle_CheckTexture); }, 10.0, false);
}

// Called every frame
void ACamActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACamActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CamReader)
	{
		CamReader->Stop();
	}

	Super::EndPlay(EndPlayReason);
}