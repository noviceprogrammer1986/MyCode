// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIREFIGHTER_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		class UHorizontalBox* PlayerStatusBox;
	
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
		class UImage* CharacterImage;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Status;

	UPROPERTY(EditAnywhere)
		FText ReadyText;

	UPROPERTY(EditAnywhere)
		FText NotReadyText;
};
