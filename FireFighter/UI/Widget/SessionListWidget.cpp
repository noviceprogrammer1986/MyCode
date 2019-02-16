// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionListWidget.h"
#include "ScrollBox.h"
#include "SessionWidget.h"
#include "OnlineSessionSettings.h"


void USessionListWidget::UpdateSessions(const TArray<class FOnlineSessionSearchResult>& Results)
{
	//CurrentSessionWidget = nullptr;
	//MyScrollBox->ClearChildren();

	for (auto& Result : Results)
	{
		USessionWidget* SessionWidget = CreateWidget<USessionWidget>(this, SessionWidgetClass);
		SessionWidget->Init(Result, this);

		MyScrollBox->AddChild(SessionWidget);
	}
}

void USessionListWidget::Clear()
{
	CurrentSessionWidget = nullptr;
	MyScrollBox->ClearChildren();
}