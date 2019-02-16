// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkMenu.h"
#include "FireFighterGameInstance.h"
#include "OnlineSessionSettings.h"
#include "Button.h"
#include "SessionListWidget.h"
#include "SessionWidget.h"
#include "CreateSessionWidget2.h"
#include "NamedSlot.h"
#include "EntryMenu2.h"
#include "Kismet/GameplayStatics.h"


void UNetworkMenu::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonCreate->OnClicked.AddDynamic(this, &UNetworkMenu::CreateGame);
	ButtonFind->OnClicked.AddDynamic(this, &UNetworkMenu::FindGame);
	ButtonJoin->OnClicked.AddDynamic(this, &UNetworkMenu::JoinGame);
	ButtonReturn->OnClicked.AddDynamic(this, &UNetworkMenu::ReturnToMain);
}

void UNetworkMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	if (!GameInst)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast FireFighterGameInstance"));
		return;
	}

	GameInst->OnFindGame.AddUObject(this, &UNetworkMenu::OnFindSuccess);
	GameInst->OnJoinFailure.AddUObject(this, &UNetworkMenu::OnJoinFail);
}

void UNetworkMenu::CreateGame()
{
	if (SubWidgetSlot->HasAnyChildren() && SubWidgetSlot->GetChildAt(0)->GetClass() == CreateSessionWidgetClass)
		return;

	SessionListWidget = nullptr;
	SubWidgetSlot->ClearChildren();
	UCreateSessionWidget2* CreateSessionWidget2 = CreateWidget<UCreateSessionWidget2>(this, CreateSessionWidgetClass);
	SubWidgetSlot->AddChild(CreateSessionWidget2);
}

void UNetworkMenu::FindGame()
{
	ButtonFind->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!SessionListWidget)
	{
		SubWidgetSlot->ClearChildren();
		SessionListWidget = CreateWidget<USessionListWidget>(this, SessionListWidgetClass);
		SubWidgetSlot->AddChild(SessionListWidget);
	}
	else
	{
		SessionListWidget->Clear();
	}

	GameInst->FindGame(*(GameInst->GetFirstGamePlayer()->GetPreferredUniqueNetId()), false);
}

void UNetworkMenu::JoinGame()
{
	if (SessionListWidget && SessionListWidget->CurrentSessionWidget)
	{
		ButtonCreate->SetVisibility(ESlateVisibility::HitTestInvisible);
		ButtonFind->SetVisibility(ESlateVisibility::HitTestInvisible);
		ButtonJoin->SetVisibility(ESlateVisibility::HitTestInvisible);
		ButtonReturn->SetVisibility(ESlateVisibility::HitTestInvisible);

		GameInst->CurrentMap.MapName = SessionListWidget->CurrentSessionWidget->ResultPtr->
			Session.SessionSettings.Settings.FindRef(SETTING_MAPNAME).Data.ToString();
		for (auto& elem : GameInst->Maps)
		{
			if (GameInst->CurrentMap.MapName == elem.MapName)
			{
				GameInst->CurrentMap.MapImage = elem.MapImage;
				break;
			}
		}

		GameInst->JoinGame(*(GameInst->GetFirstGamePlayer()->GetPreferredUniqueNetId()), GameSessionName, *(SessionListWidget->CurrentSessionWidget->ResultPtr));
	}
}

void UNetworkMenu::ReturnToMain()
{
	if (EntryMenuClass)
	{
		UEntryMenu2* EntryMenu = CreateWidget<UEntryMenu2>(UGameplayStatics::GetPlayerController(GetWorld(), 0), EntryMenuClass);
		EntryMenu->AddToViewport();
		this->RemoveFromParent();
	}
}

void UNetworkMenu::OnFindSuccess(bool bSuccess, const TArray<FOnlineSessionSearchResult>& Results)
{
	if (bSuccess && SessionListWidget)
		SessionListWidget->UpdateSessions(Results);

	ButtonFind->SetVisibility(ESlateVisibility::Visible);
}

void UNetworkMenu::OnJoinFail(EOnJoinSessionCompleteResult::Type Result)
{
	ButtonCreate->SetVisibility(ESlateVisibility::Visible);
	ButtonFind->SetVisibility(ESlateVisibility::Visible);
	ButtonJoin->SetVisibility(ESlateVisibility::Visible);
	ButtonReturn->SetVisibility(ESlateVisibility::Visible);
}
