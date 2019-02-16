// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionInterface.h"
#include "FireFighterGameInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FFindGameResultDelegate, bool, const TArray<FOnlineSessionSearchResult>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FJoinGameFailureDelegate, EOnJoinSessionCompleteResult::Type);

USTRUCT(BlueprintType)
struct FMapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FString MapName;

	UPROPERTY(EditAnywhere)
		FName MapUrl;

	UPROPERTY(EditAnywhere)
		UTexture2D* MapImage;

	UPROPERTY()
		FString ServerName;

	UPROPERTY()
		int32 NumMaxPlayers = 10;
};

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		FText CharacterName;
	UPROPERTY(EditAnywhere)
		UTexture2D* CharacterImage;
	UPROPERTY()
		FName ButtonName;
};

UCLASS()
class FIREFIGHTER_API UFireFighterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FFindGameResultDelegate OnFindGame;
	FJoinGameFailureDelegate OnJoinFailure;

	UPROPERTY()
		FMapData CurrentMap;

	UPROPERTY(EditAnywhere)
		TArray<FMapData> Maps;

	UPROPERTY()
		FString PlayerName;

	TSharedPtr<class SRPGLoadingScreen> LoadingWidget;
	
public:
	void CreateGame(const FUniqueNetId& UserID, FName SessionName, bool bUseLAN, int32 NumPublicConnections);

	void FindGame(const FUniqueNetId& UserID, bool bUseLAN);

	void JoinGame(const FUniqueNetId& UserID, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	void DestroyGame(FName SessionName);

	virtual void Init() override;
	virtual void Shutdown() override;

private:
	FOnCreateSessionCompleteDelegate CreateCompleteDelegate;
	FOnStartSessionCompleteDelegate StartCompleteDelegate;
	FDelegateHandle CreateCompleteDelegateHandle;
	FDelegateHandle StartCompleteDelegateHandle;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnFindSessionsCompleteDelegate FindCompleteDelegate;
	FDelegateHandle FindCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinCompleteDelegate;
	FDelegateHandle JoinCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroyCompleteDelegate;
	FDelegateHandle DestroyCompleteDelegateHandle;

private:
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindCompleted(bool bWasSuccessful);
	void OnJoinCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroyCompleted(FName SessionName, bool bWasSuccessful);

public:
	UFUNCTION(BlueprintCallable)
		void StartLoadingScreen(float MinPlayTime = 4.0);

	UFUNCTION(BlueprintCallable)
		void StopLoadingScreen();
};
