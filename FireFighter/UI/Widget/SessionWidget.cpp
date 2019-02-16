// Fill out your copyright notice in the Description page of Project Settings.

#include "SessionWidget.h"
#include "OnlineSessionSettings.h"
#include "Button.h"
#include "TextBlock.h"
#include "SessionListWidget.h"


void USessionWidget::Init(const FOnlineSessionSearchResult& Result, USessionListWidget* Parent)
{
	SessionListWidget = Parent;
	ResultPtr = &Result;

	MyButton->SetStyle(StyleNormal);
	MyButton->OnClicked.AddDynamic(this, &USessionWidget::OnSessionClicked);

	ServerName->SetText(FText::FromString(Result.Session.SessionSettings.Settings.FindRef(FName("SERVERNAME")).Data.ToString()));
	MapName->SetText(FText::FromString(Result.Session.SessionSettings.Settings.FindRef(SETTING_MAPNAME).Data.ToString()));

	int32 NumCurrentPlayer = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
	int32 NumMaxPlayer = Result.Session.SessionSettings.NumPublicConnections;
	Players->SetText(FText::FromString(FString::FromInt(NumCurrentPlayer) + "/" + FString::FromInt(NumMaxPlayer)));

	Ping->SetText(FText::FromString(FString::FromInt(Result.PingInMs)));
}

void USessionWidget::OnSessionClicked()
{
	if (SessionListWidget.IsValid())
	{
		if (SessionListWidget->CurrentSessionWidget && SessionListWidget->CurrentSessionWidget != this)
		{
			SessionListWidget->CurrentSessionWidget->MyButton->SetStyle(StyleNormal);
		}

		SessionListWidget->CurrentSessionWidget = this;
		MyButton->SetStyle(StyleClicked);
	}
}
