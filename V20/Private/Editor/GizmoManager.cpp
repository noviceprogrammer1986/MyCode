// Fill out your copyright notice in the Description page of Project Settings.

#include "GizmoManager.h"
#include "Engine/World.h"

#include "MyGameViewportClient.h"
#include "MyGameInstance.h"


// Sets default values
AGizmoManager::AGizmoManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AGizmoManager::BeginPlay()
{
	Super::BeginPlay();
	
	MyGameViewport = Cast<UMyGameViewportClient>(GetWorld()->GetGameViewport());
	if (!MyGameViewport)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong Viewport!"));
	}
	MyGameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if(!MyGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong GameInstance!"));
	}
}

// Called every frame
void AGizmoManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGizmoManager::EnterEditMode()
{
	MyGameViewport->EnterEditMode();
}

void AGizmoManager::ExitEditMode()
{
	MyGameViewport->ExitEditMode();
}

void AGizmoManager::SetWidgetMode(int32 Mode)
{
	MyGameViewport->SetWidgetMode(Mode);
}

void AGizmoManager::ToggleWidgetMode()
{
	MyGameViewport->ToggleWidgetMode();
}

void AGizmoManager::SetSelectedActor(AActor* SelectedActor)
{
	if (SelectedActor)
	{
		MyGameViewport->SetSelectedActor(SelectedActor);
	}
}

void AGizmoManager::ToggleCoordSystemSpace()
{
	MyGameViewport->ToggleCoordSystemSpace();
}

void AGizmoManager::DestroySelectedActor()
{
	if (MyGameViewport->SelectedActor)
	{
		MyGameInstance->SaveItems.Remove(MyGameViewport->SelectedActor);
		MyGameViewport->SelectedActor->Destroy();
		MyGameViewport->SetWidgetMode(0);
	}
}


AActor* AGizmoManager::GetSelectedActor() const
{
	if (MyGameViewport->SelectedActor)
	{
		return MyGameViewport->SelectedActor;
	}
	else
	{
		return nullptr;
	}
}
