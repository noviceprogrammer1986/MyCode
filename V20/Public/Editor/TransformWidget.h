// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TransformWidget.generated.h"


class UEditableTextBox;
class UMyGameViewportClient;

UCLASS()
class V20_API UTransformWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* LocationX;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* LocationY;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* LocationZ;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* RotationX;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* RotationY;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* RotationZ;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* ScaleX;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* ScaleY;
	UPROPERTY(meta = (BindWidget))
		UEditableTextBox* ScaleZ;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
		UMyGameViewportClient* MyGameViewport;

private:
	UFUNCTION()
		void OnLocationXCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnLocationYCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnLocationZCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnRotationXCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnRotationYCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnRotationZCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnScaleXCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnScaleYCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
		void OnScaleZCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
