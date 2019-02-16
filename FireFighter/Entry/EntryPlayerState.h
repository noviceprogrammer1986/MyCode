// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EntryPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FIREFIGHTER_API AEntryPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(replicated)
		bool bReady;

	UPROPERTY(replicated)
		FText CharacterName;

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnDeactivated() override;
};
