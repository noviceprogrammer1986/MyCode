// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionInterface.h"
#include "NetworkMenu.generated.h"

/**
 * 
 */
class UButton;
class USessionListWidget;
class UCanvasPanel;

UCLASS()
class FIREFIGHTER_API UNetworkMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* Canvas;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonCreate;
	
	UPROPERTY(meta = (BindWidget))
		UButton* ButtonFind;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonJoin;

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonReturn;

	UPROPERTY(meta = (BindWidget))
		class UNamedSlot* SubWidgetSlot;

	UPROPERTY(EditAnywhere)
		TSubclassOf<USessionListWidget> SessionListWidgetClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCreateSessionWidget2> CreateSessionWidgetClass;

	UPROPERTY()
		UClass* EntryMenuClass;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	class UFireFighterGameInstance* GameInst;

	UPROPERTY()
		USessionListWidget* SessionListWidget;

private:
	UFUNCTION()
		void CreateGame();
	UFUNCTION()
		void FindGame();
	UFUNCTION()
		void JoinGame();
	UFUNCTION()
		void ReturnToMain();

	void OnFindSuccess(bool bSucess, const TArray<class FOnlineSessionSearchResult>& Results);
	void OnJoinFail(EOnJoinSessionCompleteResult::Type Result);
};
