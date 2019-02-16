// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionListWidget.generated.h"

/**
 * 
 */
class UScrollBox;
class UHorizontalBox;
class UTextBlock;
class USessionWidget;

UCLASS()
class FIREFIGHTER_API USessionListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UScrollBox* MyScrollBox;

	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* Title;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MapName;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Players;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* Ping;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class USessionWidget> SessionWidgetClass;

	UPROPERTY()
		USessionWidget* CurrentSessionWidget;

public:
	void UpdateSessions(const TArray<class FOnlineSessionSearchResult>& Results);

	void Clear();
};
