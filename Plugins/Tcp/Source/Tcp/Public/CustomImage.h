// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "SImage.h"
#include "CustomImage.generated.h"


DECLARE_DELEGATE_OneParam(FOnImagePressed, UImage*);

class SCustomImage : public SImage
{
public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};


UCLASS()
class TCP_API UCustomImage : public UImage
{
	GENERATED_BODY()
	
public:
	virtual void SynchronizeProperties() override;

	FOnImagePressed OnImagePressed;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	UFUNCTION()
		FEventReply OnMouseButtonDown_Temp(FGeometry MyGeometry, const FPointerEvent& MouseEvent);
};
