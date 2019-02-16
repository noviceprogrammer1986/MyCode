// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovieWidget.generated.h"

/**
 * 
 */
UCLASS()
class V20_API UMovieWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* MyButton;
	
};
