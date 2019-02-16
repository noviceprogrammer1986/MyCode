// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomImage.h"


FReply SCustomImage::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return SImage::OnMouseButtonDown(MyGeometry, MouseEvent);
}

TSharedRef<SWidget> UCustomImage::RebuildWidget()
{
	MyImage = SNew(SCustomImage);
	return MyImage.ToSharedRef();
}

void UCustomImage::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	OnMouseButtonDownEvent.BindDynamic(this, &UCustomImage::OnMouseButtonDown_Temp);
}

FEventReply UCustomImage::OnMouseButtonDown_Temp(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return OnImagePressed.ExecuteIfBound(this);
	}
	else
	{
		return FEventReply();
	}
}