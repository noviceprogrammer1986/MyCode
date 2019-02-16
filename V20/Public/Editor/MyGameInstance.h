// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MySaveGame.h"
#include "MyGameInstance.generated.h"


class UUserWidget;

UCLASS()
class V20_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void LoadGameFromSlot(const FString& SlotName);
	UFUNCTION(BlueprintCallable)
		void SaveGameToSlot(const FString& SlotName);
	UFUNCTION(BlueprintCallable)
		void HideItemActors(bool bHidden);

	UFUNCTION(BlueprintCallable)
		void StartLoadingScreen();
	UFUNCTION(BlueprintCallable)
		void StopLoadingScreen();

	UPROPERTY()
		UUserWidget* LoadingScreenWidget;
	UPROPERTY()
		TMap<AActor*, FSaveItem> SaveItems;
};
