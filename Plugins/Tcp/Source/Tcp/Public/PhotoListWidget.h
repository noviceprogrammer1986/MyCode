// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PhotoListWidget.generated.h"


class UCustomScrollBox;
class UImage;
class UTexture2D;
class ULargeImageWidget;

UCLASS()
class TCP_API UPhotoListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UCustomScrollBox* MyScrollBox;
	UPROPERTY(EditDefaultsOnly)
		FVector2D ImageSize;
	UPROPERTY(EditDefaultsOnly)
		FMargin ImagePadding;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ULargeImageWidget> LargeImageWidgetClass;
	UPROPERTY(EditDefaultsOnly)
		UMaterialInterface* PhotoMaterial;

	UFUNCTION(BlueprintCallable)
		void AddImage(UTexture2D* InTexture);
	UFUNCTION(BlueprintCallable)
		void HideLargeImage();

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
		void OnImagePressed(UImage* Image);

	UFUNCTION()
		void ShowLargeImage(UImage* Image);

	UPROPERTY()
		ULargeImageWidget* MyLargeImage;
};
