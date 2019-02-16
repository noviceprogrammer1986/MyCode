// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FireFighterGameInstance.h"
#include "CreateSessionWidget2.generated.h"

/**
 * 
 */
UCLASS()
class FIREFIGHTER_API UCreateSessionWidget2 : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* ServerNameBox;
	
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* NumMaxPlayersBox;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MapName;

	UPROPERTY(meta = (BindWidget))
		class UImage* MapImage;

	UPROPERTY(meta = (BindWidget))
		class UButton* LeftButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* RightButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* CancelButton;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	FMapData CurrentMap;
	int32 CurrentIndex = 0;

	UPROPERTY()
		TArray<FMapData> Maps;

private:
	UFUNCTION()
		void SetServerName(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void SetNumMaxPlayers(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnLeftButtonClicked();
	UFUNCTION()
		void OnRightButtonClicked();
	UFUNCTION()
		void OnConfirmButtonClicked();
	UFUNCTION()
		void OnCancelButtonClicked();
};
