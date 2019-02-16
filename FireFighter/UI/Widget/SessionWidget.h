// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionWidget.generated.h"

/**
 * 
 */
class FOnlineSessionSearchResult;
class UButton;
class UHorizontalBox;
class UTextBlock;
class USessionListWidget;

UCLASS()
class FIREFIGHTER_API USessionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UButton* MyButton;

	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* NewRow;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MapName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Players;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Ping;

	UPROPERTY(EditAnywhere)
		FButtonStyle StyleNormal;

	UPROPERTY(EditAnywhere)
		FButtonStyle StyleClicked;

	const FOnlineSessionSearchResult* ResultPtr;

public:
	void Init(const FOnlineSessionSearchResult& Result, USessionListWidget* Parent);

private:
	TWeakObjectPtr<USessionListWidget> SessionListWidget;

private:
	UFUNCTION()
		void OnSessionClicked();
};
