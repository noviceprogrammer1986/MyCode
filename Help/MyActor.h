// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"


class SpeechRecognizer;

UCLASS()
class TOOLS_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void OnSpeechRecognized(FString Speech);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	TSharedPtr<SpeechRecognizer> MySpeechRecognizer;
};
