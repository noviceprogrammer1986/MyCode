//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/Image.h"
//#include "SImage.h"
//#include "ImageButton.generated.h"
//
//
//class SImageButton : public SImage
//{
//public:
//	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
//	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
//	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
//
//public:
//	TWeakObjectPtr<class UImageButton> MyOwner;
//};
//
//class UInventoryWidget;
//
//UCLASS()
//class V20_API UImageButton : public UImage
//{
//	GENERATED_BODY()
//
//protected:
//	virtual TSharedRef<SWidget> RebuildWidget() override;
//
//public:
//	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
//	void OnMouseLeave(const FPointerEvent& MouseEvent);
//	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
//	
//	void Init(UInventoryWidget* Inventory, int32 Index);
//
//public:
//	UPROPERTY()
//		UInventoryWidget* OuterWidget;
//	UPROPERTY()
//		int32 Index;
//};
