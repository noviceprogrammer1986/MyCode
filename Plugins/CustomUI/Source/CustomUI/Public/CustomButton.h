// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "CustomButton.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedEvent, UButton*, Button);

UCLASS()
class CUSTOMUI_API UCustomButton : public UButton
{
	GENERATED_BODY()

public:
	UCustomButton();

public:
	UPROPERTY()
		FOnClickedEvent OnClickedOneParam;
	
private:
	UFUNCTION()
		void OnClick();
};
