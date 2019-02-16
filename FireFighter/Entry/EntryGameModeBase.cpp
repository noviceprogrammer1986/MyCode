// Fill out your copyright notice in the Description page of Project Settings.

#include "EntryGameModeBase.h"
#include "Kismet/GameplayStatics.h"


AEntryGameModeBase::AEntryGameModeBase()
{
	
}

FString AEntryGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString Nick = UGameplayStatics::ParseOption(Options, TEXT("Nick"));
	FString NewOptions = Options;

	FString NewNick = Nick;
	if (PlayerNameSet.Contains(NewNick))
	{
		for (int i = 0; i < 10; i++)
		{
			FString TempNick = NewNick + FString::Printf(TEXT("(%d)"), i);
			if (!PlayerNameSet.Contains(TempNick))
			{
				NewNick = TempNick;
				break;
			}
		}
	}

	if (!NewNick.IsEmpty())
	{
		PlayerNameSet.Add(NewNick);

		FString OldName = UGameplayStatics::ParseOption(Options, TEXT("Name"));
		NewOptions.ReplaceInline(*(FString("?Name=") + OldName), TEXT(""));
		NewOptions.ReplaceInline(*(FString("?Nick=") + Nick), *(FString("?Name=") + NewNick));
	}

	FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, NewOptions, Portal);
	return ErrorMessage;
}

void AEntryGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}
