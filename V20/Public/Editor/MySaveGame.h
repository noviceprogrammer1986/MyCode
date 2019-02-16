// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/Actor.h"
#include "MySaveGame.generated.h"


USTRUCT()
struct FSaveItem
{
	GENERATED_BODY()
public:
	FSaveItem()
	{
		ItemClass = nullptr;
		ItemTransform = FTransform::Identity;
	}
	FSaveItem(const TSubclassOf<AActor>& InItemClass, const FTransform& InItemTransform)
	{
		ItemClass = InItemClass;
		ItemTransform = InItemTransform;
	}

	UPROPERTY()
		TSubclassOf<AActor> ItemClass;
	UPROPERTY()
		FTransform ItemTransform;
};

UCLASS()
class V20_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
		TArray<FSaveItem> SaveItems;
};
