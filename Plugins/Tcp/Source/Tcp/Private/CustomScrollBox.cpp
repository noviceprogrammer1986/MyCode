// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomScrollBox.h"
#include "ScrollBoxSlot.h"


FReply SCustomScrollBox::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MyWeakScrollBox.IsValid())
	{
		MyWeakScrollBox->OnMouseButtonReleased.ExecuteIfBound();
	}
	
	return SScrollBox::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SCustomScrollBox::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return SScrollBox::OnMouseButtonDown(MyGeometry, MouseEvent);
	//return FReply::Handled();
}

TSharedRef<SWidget> UCustomScrollBox::RebuildWidget()
{
	MyScrollBox = SNew(SCustomScrollBox)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled));

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UScrollBoxSlot* TypedSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
		}
	}

	return MyScrollBox.ToSharedRef();
}

void UCustomScrollBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyScrollBox)
	{
		static_cast<SCustomScrollBox*>(MyScrollBox.Get())->MyWeakScrollBox = this;
	}
}