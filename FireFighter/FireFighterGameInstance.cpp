// Fill out your copyright notice in the Description page of Project Settings.

#include "FireFighterGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MoviePlayer.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"


void UFireFighterGameInstance::Init()
{
	CreateCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UFireFighterGameInstance::OnCreateCompleted);
	StartCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UFireFighterGameInstance::OnStartCompleted);
	FindCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UFireFighterGameInstance::OnFindCompleted);
	JoinCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UFireFighterGameInstance::OnJoinCompleted);
	DestroyCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UFireFighterGameInstance::OnDestroyCompleted);

	if (GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		GConfig->GetString(TEXT("GameInstance"), TEXT("PlayerName"), PlayerName, GGameIni);
	}
}

void UFireFighterGameInstance::Shutdown()
{
	DestroyGame(GameSessionName);

	Super::Shutdown();
}

void UFireFighterGameInstance::CreateGame(const FUniqueNetId& UserID, FName SessionName, bool bUseLAN, int32 NumPublicConnections)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find OnlineSubsystem!"))
		return;
	}

	IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
	if (ISessionPtr.IsValid())
	{
		CreateCompleteDelegateHandle = ISessionPtr->AddOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegate);

		FOnlineSessionSettings Settings;
		Settings.bIsLANMatch = bUseLAN;
		Settings.NumPublicConnections = NumPublicConnections;
		Settings.bShouldAdvertise = true;
		Settings.bAllowJoinInProgress = true;
		Settings.bUsesPresence = true;
		Settings.bAllowJoinViaPresence = true;

		Settings.Set(SETTING_MAPNAME, CurrentMap.MapName, EOnlineDataAdvertisementType::ViaOnlineService);
		Settings.Set(FName("SERVERNAME"), CurrentMap.ServerName, EOnlineDataAdvertisementType::ViaOnlineService);

		ISessionPtr->CreateSession(UserID, SessionName, Settings);
	}
}

void UFireFighterGameInstance::OnCreateCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
		if (ISessionPtr.IsValid())
		{
			ISessionPtr->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				StartCompleteDelegateHandle = ISessionPtr->AddOnStartSessionCompleteDelegate_Handle(StartCompleteDelegate);
				ISessionPtr->StartSession(SessionName);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to Create Session"));
			}
		}
	}
}

void UFireFighterGameInstance::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
		if (ISessionPtr.IsValid())
		{
			ISessionPtr->ClearOnStartSessionCompleteDelegate_Handle(StartCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), CurrentMap.MapUrl, true, FString("listen") + "?Nick=" + PlayerName);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to Start Session"));
			}
		}
	}
}

void UFireFighterGameInstance::FindGame(const FUniqueNetId& UserID, bool bUseLAN)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find OnlineSubsystem!"))
		return;
	}

	IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
	if (ISessionPtr.IsValid())
	{
		FindCompleteDelegateHandle = ISessionPtr->AddOnFindSessionsCompleteDelegate_Handle(FindCompleteDelegate);

		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->bIsLanQuery = bUseLAN;
		SessionSearch->MaxSearchResults = 1000;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		ISessionPtr->FindSessions(UserID, SessionSearch.ToSharedRef());
	}
}

void UFireFighterGameInstance::OnFindCompleted(bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
		if (ISessionPtr.IsValid())
		{
			ISessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(FindCompleteDelegateHandle);
			if (bWasSuccessful && SessionSearch.IsValid())
			{
				if (SessionSearch->SearchResults.Num() > 0)
				{
					//Do something
					OnFindGame.Broadcast(true, SessionSearch->SearchResults);
				}
				else
				{
					OnFindGame.Broadcast(false, TArray<FOnlineSessionSearchResult>());
					UE_LOG(LogTemp, Error, TEXT("No session found!"));
				}
				
			}
			else
			{
				OnFindGame.Broadcast(false, TArray<FOnlineSessionSearchResult>());
				UE_LOG(LogTemp, Error, TEXT("Failed to Find Session"));
			}
		}
	}
}

void UFireFighterGameInstance::JoinGame(const FUniqueNetId& UserID, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find OnlineSubsystem!"))
		return;
	}

	IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
	if (ISessionPtr.IsValid())
	{
		JoinCompleteDelegateHandle = ISessionPtr->AddOnJoinSessionCompleteDelegate_Handle(JoinCompleteDelegate);
		ISessionPtr->JoinSession(UserID, SessionName, SearchResult);
	}
}

void UFireFighterGameInstance::OnJoinCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
		if (ISessionPtr.IsValid())
		{
			ISessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(JoinCompleteDelegateHandle);
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				APlayerController* PlayerController = GetFirstLocalPlayerController();
				FString TravelURL;
				if (PlayerController && ISessionPtr->GetResolvedConnectString(SessionName, TravelURL))
				{
					PlayerController->ClientTravel(TravelURL + "?Nick=" + PlayerName, TRAVEL_Absolute);
				}
			}
			else
			{
				OnJoinFailure.Broadcast(Result);
				UE_LOG(LogTemp, Error, TEXT("Failed to Join Session: %s"), LexToString(Result));
			}
		}
	}
}

void UFireFighterGameInstance::DestroyGame(FName SessionName)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find OnlineSubsystem!"))
		return;
	}

	IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
	if (ISessionPtr.IsValid())
	{
		DestroyCompleteDelegateHandle = ISessionPtr->AddOnDestroySessionCompleteDelegate_Handle(DestroyCompleteDelegate);
		ISessionPtr->DestroySession(SessionName);
	}
}

void UFireFighterGameInstance::OnDestroyCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr ISessionPtr = OnlineSub->GetSessionInterface();
		if (ISessionPtr.IsValid())
		{
			ISessionPtr->ClearOnDestroySessionCompleteDelegate_Handle(DestroyCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				//Do something
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to Destroy Session"));
			}
		}
	}
}


struct FRPGLoadingScreenBrush : public FSlateDynamicImageBrush, public FGCObject
{
	FRPGLoadingScreenBrush(const FName InTextureName, const FVector2D& InImageSize)
		: FSlateDynamicImageBrush(InTextureName, InImageSize)
	{
		SetResourceObject(LoadObject<UObject>(NULL, *InTextureName.ToString()));
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector)
	{
		if (UObject* CachedResourceObject = GetResourceObject())
		{
			Collector.AddReferencedObject(CachedResourceObject);
		}
	}
};

class SRPGLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SRPGLoadingScreen) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
	{
		// Load version of the logo with text baked in, path is hardcoded because this loads very early in startup
		static const FName LoadingScreenName(TEXT("/Game/_Core/loading.loading"));

		LoadingScreenBrush = MakeShareable(new FRPGLoadingScreenBrush(LoadingScreenName, FVector2D(1920, 1080)));

		FSlateBrush *BGBrush = new FSlateBrush();
		BGBrush->TintColor = FLinearColor(0.034f, 0.034f, 0.034f, 1.0f);

		ChildSlot
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(BGBrush)
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(LoadingScreenBrush.Get())
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.Padding(FMargin(10.0f, 150.0f, 10.0f, 10.0f))
			[
				SNew(SThrobber)
				//.Visibility(this, &SRPGLoadingScreen::GetLoadIndicatorVisibility)
			]
			]
			];
	}

private:
	/** Rather to show the ... indicator */
	EVisibility GetLoadIndicatorVisibility() const
	{
		bool Vis = GetMoviePlayer()->IsLoadingFinished();
		return GetMoviePlayer()->IsLoadingFinished() ? EVisibility::Collapsed : EVisibility::Visible;
	}

	/** Loading screen image brush */
	TSharedPtr<FSlateDynamicImageBrush> LoadingScreenBrush;
};

void UFireFighterGameInstance::StartLoadingScreen(float MinPlayTime)
{
	//FLoadingScreenAttributes LoadingScreen;
	//LoadingScreen.MinimumLoadingScreenDisplayTime = MinPlayTime;
	//LoadingScreen.WidgetLoadingScreen = SNew(SRPGLoadingScreen);
	//GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

	LoadingWidget = SNew(SRPGLoadingScreen);
	GetEngine()->GameViewport->AddViewportWidgetContent(LoadingWidget.ToSharedRef(), 10);
}

void UFireFighterGameInstance::StopLoadingScreen()
{
	if (LoadingWidget.IsValid())
	{
		GetEngine()->GameViewport->RemoveViewportWidgetContent(LoadingWidget.ToSharedRef());
	}
}
