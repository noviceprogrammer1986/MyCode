// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovieContainer.generated.h"


class UWrapBox;
class UMovieWidget;

UCLASS()
class V20_API UMovieContainer : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
		void Init(const TArray<FString>& InVideoFiles);

public:
	UPROPERTY(meta = (BindWidget))
		UWrapBox* MyWrapBox;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UMovieWidget> MovieWidgetClassBP;

private:
	UPROPERTY()
		FVector2D WidgetSize;
	UPROPERTY()
		TArray<FString> VideoFiles;
	UPROPERTY()
		int32 Row;
	UPROPERTY()
		int32 Column;
	UPROPERTY()
		int32 CurrentIndex;
};
