// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlateTypes.h"
#include "FireFighterGameInstance.h"
#include "LobbyRoomMenu.generated.h"

/**
 * 
 */
UCLASS()
class FIREFIGHTER_API ULobbyRoomMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UScrollBox* ChatRecord;

	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ChatBox;
	
	UPROPERTY(meta = (BindWidget))
		class UUniformGridPanel* CharacterContainer;

	UPROPERTY(meta = (BindWidget))
		class UBorder* ChracterImageBorder;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CharacterNameText;

	UPROPERTY(meta = (BindWidget))
		class UBorder* MapImageBorder;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MapNameText;

	UPROPERTY(meta = (BindWidget))
		class UVerticalBox* PlayerStatusPanel;

	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonReady;

	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonStart;

	UPROPERTY(meta = (BindWidget))
		class UButton* ButtonReturn;

	UPROPERTY(EditAnywhere)
		FButtonStyle CharacterButtonStyle;

	UPROPERTY(EditAnywhere)
		TArray<FCharacterData> Characters;

	UPROPERTY(EditAnywhere)
		int32 MaxCharactersPerRow = 5;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPlayerStatusWidget> PlayerStatusWidgetClass;

	UPROPERTY(EditAnywhere)
		FText ErrorText;

	UPROPERTY(EditAnywhere)
		FString CountDownStr;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
		class AFireFighterPlayerController* PlayerController;
	UPROPERTY()
		class UFireFighterGameInstance* GameInst;

	bool bAllPlayersReady = false;
	FTimerHandle CountDownHandle;
	int32 Counter = 3;

private:
	UFUNCTION()
		void OnCharacterButtonClicked(class UButton* Button);

	UFUNCTION()
		void CommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
		void OnReceivedChatMessage(const FString& PlayerName, const FText& ChatMessage);

	UFUNCTION()
		void UpdatePlayerStatusPanel();

	UFUNCTION()
		void OnReadyButtonClicked();

	UFUNCTION()
		void OnStartButtonClicked();

	UFUNCTION()
		void OnReturnButtonClicked();

	UFUNCTION()
		void BeginCountDown();
};
