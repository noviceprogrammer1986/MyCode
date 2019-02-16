// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VideoPlayerActor.generated.h"


class UMediaPlayer;
class UMediaTexture;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

UCLASS()
class V20_API AVideoPlayerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVideoPlayerActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
		void LoadVideoFiles();
	UFUNCTION()
		void PlayMovie();
	
public:
	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* MovieScene;
	UPROPERTY(EditAnywhere)
		FString VideoFilesPath;

private:
	UPROPERTY()
		UMediaPlayer* MyMediaPlayer;
	UPROPERTY()
		UMediaTexture* MyMediaTexture;
	UPROPERTY()
		UMaterialInstanceDynamic* MID;
	UPROPERTY()
		TArray<FString> VideoFiles;

	FTimerHandle TempHandle;
	FTimerHandle TempClearHandle;
};
