//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "ImageButton.h"
//#include "InventoryWidget.h"
//
//
//void SImageButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	bIsHovered = true;
//	if (MyOwner.IsValid())
//	{
//		MyOwner->OnMouseEnter(MyGeometry, MouseEvent);
//	}
//}
//
//void SImageButton::OnMouseLeave(const FPointerEvent& MouseEvent)
//{
//	bIsHovered = false;
//	if (MyOwner.IsValid())
//	{
//		MyOwner->OnMouseLeave(MouseEvent);
//	}
//}
//
//FReply SImageButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	if (MyOwner.IsValid() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
//	{
//		return MyOwner->OnMouseButtonDown(MyGeometry, MouseEvent);
//	}
//	return FReply::Unhandled();
//}
//
//TSharedRef<SWidget> UImageButton::RebuildWidget()
//{
//	TSharedPtr<SImageButton> MyImageButton = SNew(SImageButton);
//	MyImageButton->MyOwner = this;
//	MyImage = MyImageButton;
//	return MyImage.ToSharedRef();
//}
//
//void UImageButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
//	{
//		SetBrushTintColor(FLinearColor::Gray);
//	}
//}
//
//void UImageButton::OnMouseLeave(const FPointerEvent& MouseEvent)
//{
//	SetBrushTintColor(FLinearColor::White);
//}
//
//FReply UImageButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	if (OuterWidget)
//	{
//		SetBrushTintColor(FLinearColor::Yellow);
//		OuterWidget->CurrentButton = this;
//		return FReply::Handled().DetectDrag(OuterWidget->TakeWidget(), EKeys::LeftMouseButton);
//	}
//	return FReply::Unhandled();
//}
//
//void UImageButton::Init(UInventoryWidget* Inventory, int32 InIndex)
//{
//	OuterWidget = Inventory;
//	Index = InIndex;
//	SetBrushFromTexture(OuterWidget->Items[Index].Icon);
//	Brush.ImageSize = OuterWidget->IconSize;
//}