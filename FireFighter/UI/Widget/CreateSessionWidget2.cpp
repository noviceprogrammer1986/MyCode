// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateSessionWidget2.h"
#include "EditableTextBox.h"
#include "Button.h"
#include "TextBlock.h"
#include "Image.h"
#include "Engine/Texture2D.h"
#include "FireFighterGameInstance.h"


void UCreateSessionWidget2::NativePreConstruct()
{
	Super::NativePreConstruct();

	ServerNameBox->OnTextCommitted.AddDynamic(this, &UCreateSessionWidget2::SetServerName);
	NumMaxPlayersBox->OnTextCommitted.AddDynamic(this, &UCreateSessionWidget2::SetNumMaxPlayers);
	LeftButton->OnClicked.AddDynamic(this, &UCreateSessionWidget2::OnLeftButtonClicked);
	RightButton->OnClicked.AddDynamic(this, &UCreateSessionWidget2::OnRightButtonClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UCreateSessionWidget2::OnConfirmButtonClicked);
	CancelButton->OnClicked.AddDynamic(this, &UCreateSessionWidget2::OnCancelButtonClicked);
}

void UCreateSessionWidget2::NativeConstruct()
{
	Super::NativeConstruct();

	UFireFighterGameInstance* GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	Maps = GameInst->Maps;
}

void UCreateSessionWidget2::SetServerName(const FText & Text, ETextCommit::Type CommitMethod)
{
	CurrentMap.ServerName = Text.ToString();
}

void UCreateSessionWidget2::SetNumMaxPlayers(const FText & Text, ETextCommit::Type CommitMethod)
{
	CurrentMap.NumMaxPlayers = FMath::Clamp(FCString::Atoi(*Text.ToString()), 0, 100);
	NumMaxPlayersBox->SetText(FText::FromString(FString::FromInt(CurrentMap.NumMaxPlayers)));
}

void UCreateSessionWidget2::OnLeftButtonClicked()
{
	CurrentIndex = (CurrentIndex - 1 + Maps.Num()) % (Maps.Num());
	MapName->SetText(FText::FromString(Maps[CurrentIndex].MapName));
	MapImage->Brush.SetResourceObject(Maps[CurrentIndex].MapImage);
}

void UCreateSessionWidget2::OnRightButtonClicked()
{
	CurrentIndex = (CurrentIndex + 1) % (Maps.Num());
	MapName->SetText(FText::FromString(Maps[CurrentIndex].MapName));
	MapImage->Brush.SetResourceObject(Maps[CurrentIndex].MapImage);
}

void UCreateSessionWidget2::OnConfirmButtonClicked()
{
	if (CurrentMap.ServerName.IsEmpty() || CurrentMap.NumMaxPlayers == 0)
		return;

	CurrentMap.MapName = Maps[CurrentIndex].MapName;
	CurrentMap.MapUrl = Maps[CurrentIndex].MapUrl;
	CurrentMap.MapImage = Maps[CurrentIndex].MapImage;
	UFireFighterGameInstance* GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	GameInst->CurrentMap = CurrentMap;
	GameInst->CreateGame(*(GameInst->GetFirstGamePlayer()->GetPreferredUniqueNetId()), GameSessionName, false, CurrentMap.NumMaxPlayers);
}

void UCreateSessionWidget2::OnCancelButtonClicked()
{
	RemoveFromParent();
}
