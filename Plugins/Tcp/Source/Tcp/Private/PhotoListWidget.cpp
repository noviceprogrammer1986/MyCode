// Fill out your copyright notice in the Description page of Project Settings.

#include "PhotoListWidget.h"
#include "ScrollBoxSlot.h"
#include "Engine/Texture2D.h"

#include "CustomScrollBox.h"
#include "CustomImage.h"
#include "LargeImageWidget.h"


void UPhotoListWidget::AddImage(UTexture2D* InTexture)
{
	UCustomImage* NewImage = NewObject<UCustomImage>(this);
	NewImage->Brush.SetResourceObject(InTexture);
	NewImage->Brush.ImageSize = ImageSize;

	NewImage->OnImagePressed.BindUObject(this, &UPhotoListWidget::OnImagePressed);

	UScrollBoxSlot* MySlot = Cast<UScrollBoxSlot>(MyScrollBox->AddChild(NewImage));
	MySlot->SetPadding(FMargin(ImagePadding));
	MyScrollBox->ScrollToEnd();
}

void UPhotoListWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UPhotoListWidget::OnImagePressed(UImage* Image)
{
	ShowLargeImage(Image);
}

void UPhotoListWidget::ShowLargeImage(UImage* Image)
{
	if (!MyLargeImage)
	{
		MyLargeImage = CreateWidget<ULargeImageWidget>(this, LargeImageWidgetClass);
		MyLargeImage->AddToViewport();
	}
	MyLargeImage->LargeImage->Brush.SetResourceObject(Image->Brush.GetResourceObject());
}

void UPhotoListWidget::HideLargeImage()
{
	if (MyLargeImage)
	{
		MyLargeImage->RemoveFromViewport();
		MyLargeImage = nullptr;
	}
}