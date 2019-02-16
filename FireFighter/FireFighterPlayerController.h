// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FireFighterPlayerController.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FReceivedChatMessageDelegate, const FString&, const FText&);
DECLARE_DELEGATE(FBeginCountDown);

UCLASS()
class FIREFIGHTER_API AFireFighterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, UnReliable, WithValidation)
		void ServerHandleChatMessage(const FString& PlayerName, const FText& ChatMessage);
	
	UFUNCTION(Client, UnReliable)
		void ClientHandleChatMessage(const FString& PlayerName, const FText& ChatMessage);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetCharacterName(const FText& CharacterName);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPlayerStatus(bool bReady);

	UFUNCTION()
		void ServerLeaveSession();

	UFUNCTION(Client, Reliable)
		void ClientLeaveSession();

	UFUNCTION()
		void ServerNotifyCountDown();

	UFUNCTION(Client, Reliable)
		void ClientCountDown();

public:
	FTimerHandle Handle;

	FReceivedChatMessageDelegate OnReceivedChatMessage;

	FBeginCountDown OnBeginCountDown;

protected:
	virtual void SetupInputComponent() override;
};
