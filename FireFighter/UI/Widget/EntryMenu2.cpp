// Fill out your copyright notice in the Description page of Project Settings.

#include "EntryMenu2.h"
#include "Button.h"
#include "EditableTextBox.h"
#include "FireFighterGameInstance.h"
#include "NetworkMenu.h"
#include "Kismet/GameplayStatics.h"
#include "ConfigCacheIni.h"


void UEntryMenu2::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonStart->OnClicked.AddDynamic(this, &UEntryMenu2::OnButtonStartClicked);
	ButtonQuit->OnClicked.AddDynamic(this, &UEntryMenu2::OnButtonQuitClicked);
	NameBox->OnTextCommitted.AddDynamic(this, &UEntryMenu2::OnPlayerNameCommitted);
}

void UEntryMenu2::NativeConstruct()
{
	UFireFighterGameInstance* GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	if (GameInst && !GameInst->PlayerName.IsEmpty())
	{
		NameBox->SetText(FText::FromString(GameInst->PlayerName));
		ButtonStart->SetVisibility(ESlateVisibility::Visible);
	}
}

void UEntryMenu2::OnButtonStartClicked()
{
	if (NetworkMenuClass->IsValidLowLevelFast())
	{
		UNetworkMenu* NetworkMenu = CreateWidget<UNetworkMenu>(UGameplayStatics::GetPlayerController(GetWorld(), 0), NetworkMenuClass);
		NetworkMenu->EntryMenuClass = this->GetClass();
		NetworkMenu->AddToViewport();
		this->RemoveFromParent();
	}
}

void UEntryMenu2::OnButtonQuitClicked()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("quit");
}

void UEntryMenu2::OnPlayerNameCommitted(const FText & Text, ETextCommit::Type CommitMethod)
{
	if (Text.IsEmpty())
	{
		ButtonStart->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UFireFighterGameInstance* GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
		if (GameInst)
		{
			GameInst->PlayerName = Text.ToString();
			GConfig->SetString(TEXT("GameInstance"), TEXT("PlayerName"), *GameInst->PlayerName, GGameIni);
			ButtonStart->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
