// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EntryMenu2.generated.h"

/**
 * 
 */
class UButton;
class UEditableTextBox;

UCLASS()
class FIREFIGHTER_API UEntryMenu2 : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UButton* ButtonStart;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonQuit;
	
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* NameBox;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UNetworkMenu> NetworkMenuClass;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnButtonStartClicked();

	UFUNCTION()
		void OnButtonQuitClicked();

	UFUNCTION()
		void OnPlayerNameCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
