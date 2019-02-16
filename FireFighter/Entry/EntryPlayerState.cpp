// Fill out your copyright notice in the Description page of Project Settings.

#include "EntryPlayerState.h"
#include "UnrealNetwork.h"
#include "EntryGameModeBase.h"
#include "Engine/World.h"


void AEntryPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEntryPlayerState, bReady);
	DOREPLIFETIME(AEntryPlayerState, CharacterName);
}

void AEntryPlayerState::OnDeactivated()
{
	AEntryGameModeBase* EntryGameMode = Cast<AEntryGameModeBase>(GetWorld()->GetAuthGameMode());
	if (EntryGameMode)
	{
		EntryGameMode->PlayerNameSet.Remove(GetPlayerName());
	}

	Super::OnDeactivated();
}

