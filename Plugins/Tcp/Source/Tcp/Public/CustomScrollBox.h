// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ScrollBox.h"
#include "CustomScrollBox.generated.h"


DECLARE_DELEGATE(FOnMouseButtonReleased)

class UCustomScrollBox;

class SCustomScrollBox : public SScrollBox
{
public:
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	TWeakObjectPtr<UCustomScrollBox> MyWeakScrollBox;
};


UCLASS()
class TCP_API UCustomScrollBox : public UScrollBox
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

	FOnMouseButtonReleased OnMouseButtonReleased;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
