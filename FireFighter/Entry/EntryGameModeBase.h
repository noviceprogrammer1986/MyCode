// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EntryGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class FIREFIGHTER_API AEntryGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AEntryGameModeBase();

protected:
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual void BeginPlay() override;

public:
	UPROPERTY()
		TSet<FString> PlayerNameSet;
};
