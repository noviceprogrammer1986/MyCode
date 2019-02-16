// Fill out your copyright notice in the Description page of Project Settings.

#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UserWidget.h"


void UMyGameInstance::LoadGameFromSlot(const FString& SlotName)
{
	UMySaveGame* MySaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!MySaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot load saved game from %s"), *SlotName);
		return;
	}
	for (const FSaveItem& Item : MySaveGame->SaveItems)
	{
		if (Item.ItemClass != nullptr)
		{
			AActor* ItemActor = GetWorld()->SpawnActor(Item.ItemClass, &Item.ItemTransform);
			SaveItems.Add(ItemActor, Item);
		}
	}
}

void UMyGameInstance::SaveGameToSlot(const FString& SlotName)
{
	UMySaveGame* MySaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
	SaveItems.GenerateValueArray(MySaveGame->SaveItems);
	UGameplayStatics::SaveGameToSlot(MySaveGame, SlotName, 0);
}

void UMyGameInstance::StartLoadingScreen()
{
	UClass* MyWidgetClass = LoadClass<UUserWidget>(NULL, TEXT("/Game/MAPS/WBP_Loading.WBP_Loading_C"));
	if (MyWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<UUserWidget>(this, MyWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(100);
			//GetGameViewportClient()->AddViewportWidgetContent(LoadingScreenWidget->TakeWidget(), 0);
			//GetWorld()->ServerTravel("Level0");
			GetWorld()->SeamlessTravel("Level0");
		}
	}
}

void UMyGameInstance::StopLoadingScreen()
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->RemoveFromParent();
	}
}

void UMyGameInstance::HideItemActors(bool bHidden)
{
	TArray<AActor*> ItemActors;
	SaveItems.GetKeys(ItemActors);
	for (const auto& Item : ItemActors)
	{
		Item->SetActorHiddenInGame(bHidden);
	}
}