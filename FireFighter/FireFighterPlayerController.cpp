// Fill out your copyright notice in the Description page of Project Settings.

#include "FireFighterPlayerController.h"
#include "EngineUtils.h"
#include "Entry/EntryPlayerState.h"
#include "FireFighterGameInstance.h"
#include "GameMapsSettings.h"
#include "TimerManager.h"


void AFireFighterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("StartTalking", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("StopTalking", IE_Pressed, this, &APlayerController::StopTalking);
}

void AFireFighterPlayerController::ServerHandleChatMessage_Implementation(const FString & PlayerName, const FText & ChatMessage)
{
	for (TActorIterator<AFireFighterPlayerController> Itr(GetWorld()); Itr; ++Itr)
	{
		(*Itr)->ClientHandleChatMessage(PlayerName, ChatMessage);
	}
}

bool AFireFighterPlayerController::ServerHandleChatMessage_Validate(const FString & PlayerName, const FText & ChatMessage)
{
	return true;
}

void AFireFighterPlayerController::ClientHandleChatMessage_Implementation(const FString & PlayerName, const FText & ChatMessage)
{
	OnReceivedChatMessage.ExecuteIfBound(PlayerName, ChatMessage);
}

bool AFireFighterPlayerController::ServerSetCharacterName_Validate(const FText& CharacterName)
{
	return true;
}

void AFireFighterPlayerController::ServerSetCharacterName_Implementation(const FText& CharacterName)
{
	AEntryPlayerState* PS = Cast<AEntryPlayerState>(PlayerState);
	if (PS)
	{
		PS->CharacterName = CharacterName;
	}
}

bool AFireFighterPlayerController::ServerSetPlayerStatus_Validate(bool bReady)
{
	return true;
}

void AFireFighterPlayerController::ServerSetPlayerStatus_Implementation(bool bReady)
{
	AEntryPlayerState* PS = Cast<AEntryPlayerState>(PlayerState);
	if (PS)
	{
		PS->bReady = bReady;
	}
}

void AFireFighterPlayerController::ServerLeaveSession()
{
	for (TActorIterator<AFireFighterPlayerController> Itr(GetWorld()); Itr; ++Itr)
	{
		(*Itr)->ClientLeaveSession();
	}
}

void AFireFighterPlayerController::ClientLeaveSession_Implementation()
{
	UFireFighterGameInstance* GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	if (GameInst)
	{
		GetWorldTimerManager().ClearTimer(Handle);

		GameInst->DestroyGame(GameSessionName);
		ClientTravel(UGameMapsSettings::GetGameDefaultMap(), TRAVEL_Absolute);
	}
}

void AFireFighterPlayerController::ServerNotifyCountDown()
{
	for (TActorIterator<AFireFighterPlayerController> Itr(GetWorld()); Itr; ++Itr)
	{
		(*Itr)->ClientCountDown();
	}
}

void AFireFighterPlayerController::ClientCountDown_Implementation()
{
	OnBeginCountDown.ExecuteIfBound();
}