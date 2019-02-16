// Fill out your copyright notice in the Description page of Project Settings.

#include "MovieContainer.h"
#include "WrapBox.h"
#include "MovieWidget.h"
#include "Button.h"
#include "CanvasPanelSlot.h"
#include "ConfigCacheIni.h"


void UMovieContainer::Init(const TArray<FString>& InVideoFiles)
{
	if (InVideoFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("no video files found!"));
		return;
	}
	VideoFiles = InVideoFiles;
	if (!MovieWidgetClassBP)
	{
		UE_LOG(LogTemp, Error, TEXT("moviewidgetclassbp not set!"));
		return;
	}

	UMovieWidget* FirstMovieWidget = CreateWidget<UMovieWidget>(this, MovieWidgetClassBP);
	UCanvasPanelSlot* MyChildSlot = Cast<UCanvasPanelSlot>(FirstMovieWidget->MyButton->Slot);
	if (!MyChildSlot)
	{
		UE_LOG(LogTemp, Error, TEXT("missing canvaspanel"));
		return;
	}

	GConfig->GetVector2D(TEXT("/Script/V20.MovieContainer"), TEXT("WidgetSize"), WidgetSize, GGameIni);
	if (WidgetSize == FVector2D(0, 0))
	{
		WidgetSize = MyChildSlot->GetSize();
		GConfig->SetVector2D(TEXT("/Script/V20.MovieContainer"), TEXT("WidgetSize"), WidgetSize, GGameIni);
	}
	UCanvasPanelSlot* MySlot = Cast<UCanvasPanelSlot>(MyWrapBox->Slot);
	if (!MySlot)
	{
		UE_LOG(LogTemp, Error, TEXT("missing canvaspanel"));
		return;
	}
	Column = MySlot->GetSize().X / (WidgetSize.X + MyWrapBox->InnerSlotPadding.X);
	Row = MySlot->GetSize().Y / (WidgetSize.Y + MyWrapBox->InnerSlotPadding.Y);

	int32 NumWidgets = FMath::Min(VideoFiles.Num(), Column*Row);
	for (int32 i = 1; i < NumWidgets; ++i)
	{
		UMovieWidget* NewMovieWidget = CreateWidget<UMovieWidget>(this, MovieWidgetClassBP);
		MyWrapBox->AddChild(NewMovieWidget);
	}
}