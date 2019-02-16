// Fill out your copyright notice in the Description page of Project Settings.

#include "LobbyRoomMenu.h"
#include "CustomButton.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"
#include "Engine/Texture2D.h"
#include "CustomTextBlock.h"
#include "Border.h"
#include "EditableTextBox.h"
#include "ScrollBox.h"
#include "FireFighterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "GameFramework/GameStateBase.h"
#include "Entry/EntryPlayerState.h"
#include "FireFighterGameInstance.h"
#include "PlayerStatusWidget.h"
#include "VerticalBox.h"
#include "Image.h"
#include "Engine/AssetManager.h"


void ULobbyRoomMenu::NativePreConstruct()
{
	Super::NativePreConstruct();

	for (int32 i = 0; i < Characters.Num(); ++i)
	{
		UCustomButton* NewButton = NewObject<UCustomButton>(this);
		NewButton->SetStyle(CharacterButtonStyle);
		UUniformGridSlot* MySlot = CharacterContainer->AddChildToUniformGrid(NewButton);
		MySlot->SetRow(i / MaxCharactersPerRow);
		MySlot->SetColumn(i % MaxCharactersPerRow);

		NewButton->WidgetStyle.Normal.SetResourceObject(Characters[i].CharacterImage);
		NewButton->WidgetStyle.Hovered.SetResourceObject(Characters[i].CharacterImage);
		NewButton->WidgetStyle.Pressed.SetResourceObject(Characters[i].CharacterImage);

		NewButton->OnClickedOneParam.AddDynamic(this, &ULobbyRoomMenu::OnCharacterButtonClicked);
		Characters[i].ButtonName = NewButton->GetFName();
	}
}

void ULobbyRoomMenu::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst = Cast<UFireFighterGameInstance>(GetGameInstance());
	if (GameInst)
	{
		MapImageBorder->Background.SetResourceObject(GameInst->CurrentMap.MapImage);
		MapNameText->SetText(FText::FromString(GameInst->CurrentMap.MapName));
	}

	ChatBox->OnTextCommitted.AddDynamic(this, &ULobbyRoomMenu::CommitChatMessage);
	ButtonReady->OnClicked.AddDynamic(this, &ULobbyRoomMenu::OnReadyButtonClicked);
	ButtonReturn->OnClicked.AddDynamic(this, &ULobbyRoomMenu::OnReturnButtonClicked);

	PlayerController = Cast<AFireFighterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController)
	{
		PlayerController->OnReceivedChatMessage.BindUObject(this, &ULobbyRoomMenu::OnReceivedChatMessage);
		PlayerController->OnBeginCountDown.BindUObject(this, &ULobbyRoomMenu::BeginCountDown);
		GetWorld()->GetTimerManager().SetTimer(PlayerController->Handle, [&] { UpdatePlayerStatusPanel(); }, 0.1, true);
	}

	if (GetWorld()->GetNetMode() < ENetMode::NM_Client)
	{
		ButtonStart->SetVisibility(ESlateVisibility::Visible);
		ButtonStart->OnClicked.AddDynamic(this, &ULobbyRoomMenu::OnStartButtonClicked);
	}
}

void ULobbyRoomMenu::OnCharacterButtonClicked(UButton* Button)
{
	for (const auto& elem : Characters)
	{
		if (Button->GetFName() == elem.ButtonName)
		{
			CharacterNameText->SetText(elem.CharacterName);
			ChracterImageBorder->SetBrushFromTexture(elem.CharacterImage);
			if (PlayerController)
			{
				PlayerController->ServerSetCharacterName(elem.CharacterName);
			}
			break;
		}
	}
}

void ULobbyRoomMenu::CommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!Text.IsEmpty())
		{
			if (PlayerController)
			{
				PlayerController->ServerHandleChatMessage(PlayerController->PlayerState->GetPlayerName(), Text);
				ChatBox->SetText(FText());
			}
		}
	}
}

void ULobbyRoomMenu::OnReceivedChatMessage(const FString& PlayerName, const FText& ChatMessage)
{
	UCustomTextBlock* NewText = NewObject<UCustomTextBlock>(this);
	NewText->SetText(FText::FromString(PlayerName + ": " + ChatMessage.ToString()));
	ChatRecord->AddChild(NewText);
	ChatRecord->ScrollToEnd();
}

void ULobbyRoomMenu::UpdatePlayerStatusPanel()
{
	if (!PlayerStatusWidgetClass->IsValidLowLevelFast())
		return;

	PlayerStatusPanel->ClearChildren();

	TArray<AEntryPlayerState*> PlayerArray;
	for (auto& elem : UGameplayStatics::GetGameState(GetWorld())->PlayerArray)
	{
		AEntryPlayerState* PS = Cast<AEntryPlayerState>(elem);
		if (PS)
		{
			PlayerArray.Add(PS);
		}
		else
		{
			return;
		}
	}

	PlayerArray.Sort([&](const AEntryPlayerState& A, const AEntryPlayerState& B) { return A.StartTime < B.StartTime; });

	bAllPlayersReady = true;
	for (auto& elem : PlayerArray)
	{
		UPlayerStatusWidget* PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(this, PlayerStatusWidgetClass);
		PlayerStatusWidget->PlayerName->SetText(FText::FromString(elem->GetPlayerName()));

		for (auto& cha : Characters)
		{
			if (elem->CharacterName.EqualTo(cha.CharacterName))
			{
				PlayerStatusWidget->CharacterImage->Brush.SetResourceObject(cha.CharacterImage);
				break;
			}
		}

		if (elem->bReady)
			PlayerStatusWidget->Status->SetText(PlayerStatusWidget->ReadyText);
		else
		{
			PlayerStatusWidget->Status->SetText(PlayerStatusWidget->NotReadyText);
			bAllPlayersReady = false;
		}

		PlayerStatusPanel->AddChild(PlayerStatusWidget);
	}
}

void ULobbyRoomMenu::OnReadyButtonClicked()
{
	if (PlayerController)
	{
		PlayerController->ServerSetPlayerStatus(true);
	}
}

void ULobbyRoomMenu::OnStartButtonClicked()
{
	if (bAllPlayersReady && PlayerController)
	{
		ButtonStart->SetVisibility(ESlateVisibility::HitTestInvisible);
		PlayerController->ServerNotifyCountDown();
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, [&] { GetWorld()->ServerTravel("RIVERScene8"); }, 4.0, false);
	}
	else
	{
		UTextBlock* ErrorMessage = NewObject<UTextBlock>(this);
		ErrorMessage->SetText(ErrorText);
		ErrorMessage->SetColorAndOpacity(FSlateColor(FLinearColor(1.0, 0.0, 0.0)));
		ChatRecord->AddChild(ErrorMessage);
	}
}

void ULobbyRoomMenu::OnReturnButtonClicked()
{
	if (GetWorld()->GetNetMode() < NM_Client)
	{
		PlayerController->ServerLeaveSession();
	}
	else
	{
		PlayerController->ClientLeaveSession();
	}
}

void ULobbyRoomMenu::BeginCountDown()
{
	GetWorld()->GetTimerManager().ClearTimer(PlayerController->Handle);

	GetWorld()->GetTimerManager().SetTimer(CountDownHandle, [&]
	{
		UTextBlock* CountDownMessage = NewObject<UTextBlock>(this);
		CountDownMessage->SetText(FText::FromString(CountDownStr.Replace(TEXT("x"), *FString::FromInt(Counter))));
		CountDownMessage->SetColorAndOpacity(FSlateColor(FLinearColor(0.0, 1.0, 0.0)));
		ChatRecord->AddChild(CountDownMessage);
		--Counter;
	},
		1.0, true);

	FTimerHandle TempHandle;
	GetWorld()->GetTimerManager().SetTimer(TempHandle, [&] 
	{ 
		GetWorld()->GetTimerManager().ClearTimer(CountDownHandle); 
		GameInst->StartLoadingScreen();
		this->RemoveFromParent();
	}, 3.5, false);
}
