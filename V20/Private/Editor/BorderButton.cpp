// Fill out your copyright notice in the Description page of Project Settings.

#include "BorderButton.h"
#include "Components/BorderSlot.h"
#include "InventoryWidget.h"
#include "SizeBox.h"


void SBorderButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsHovered = true;
	if (MyOwner.IsValid())
	{
		MyOwner->OnMouseEnter(MyGeometry, MouseEvent);
	}
}

void SBorderButton::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	bIsHovered = false;
	if (MyOwner.IsValid())
	{
		MyOwner->OnMouseLeave(MouseEvent);
	}
}

FReply SBorderButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MyOwner.IsValid() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return MyOwner->OnMouseButtonDown(MyGeometry, MouseEvent);
	}
	return FReply::Unhandled();
}

TSharedRef<SWidget> UBorderButton::RebuildWidget()
{
	TSharedPtr<SBorderButton> MyBorderButton = SNew(SBorderButton);
	MyBorderButton->MyOwner = this;
	MyBorder = MyBorderButton;
	if (GetChildrenCount() > 0)
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}
	return MyBorder.ToSharedRef();
}

void UBorderButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		SetBrushColor(FLinearColor::Gray);
	}
}

void UBorderButton::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SetBrushColor(FLinearColor::White);
}

FReply UBorderButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (OuterWidget)
	{
		SetBrushColor(FLinearColor::Yellow);
		OuterWidget->CurrentButton = this;
		return FReply::Handled().DetectDrag(OuterWidget->TakeWidget(), EKeys::LeftMouseButton);
	}
	return FReply::Unhandled();
}

void UBorderButton::Init(UInventoryWidget* Inventory, int32 InIndex)
{
	OuterWidget = Inventory;
	Index = InIndex;
	Background.ImageSize = OuterWidget->IconSize;
	SetPadding(FMargin(0));

	USizeBox* NewSizeBox = NewObject<USizeBox>(this);
	NewSizeBox->SetWidthOverride(OuterWidget->IconSize.X);
	NewSizeBox->SetHeightOverride(OuterWidget->IconSize.Y);
	AddChild(NewSizeBox);

	if (Index < OuterWidget->Items.Num())
	{
		SetBrushFromTexture(OuterWidget->Items[Index].Icon);
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}