// Fill out your copyright notice in the Description page of Project Settings.

#include "VideoPlayerActor.h"
#include "Paths.h"
#include "FileManager.h"
#include "MediaAssets/Public/MediaPlayer.h"
#include "MediaAssets/Public/MediaTexture.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "MovieContainer.h"
#include "UObjectIterator.h"


// Sets default values
AVideoPlayerActor::AVideoPlayerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MovieScene = CreateDefaultSubobject<UStaticMeshComponent>("MovieScene");
	SetRootComponent(MovieScene);
}

// Called when the game starts or when spawned
void AVideoPlayerActor::BeginPlay()
{
	Super::BeginPlay();
	
	LoadVideoFiles();

	MyMediaPlayer = LoadObject<UMediaPlayer>(this, TEXT("/Game/_Test/NewMediaPlayer.NewMediaPlayer"));
	if (!MyMediaPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("failed to load mediaplayer"));
		return;
	}
	MyMediaTexture = LoadObject<UMediaTexture>(this, TEXT("/Game/_Test/NewMediaTexture.NewMediaTexture"));
	if (!MyMediaTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("failed to load mediatexture"));
		return;
	}

	if (MovieScene)
	{
		MID = MovieScene->CreateDynamicMaterialInstance(0);
		MID->SetScalarParameterValue("MovieAspectRatio", 2.0);
	}

	//PlayMovie();
	for (TObjectIterator<UMovieContainer> Itr; Itr; ++Itr)
	{
		if (Itr->GetWorld() == GetWorld())
		{
			Itr->Init(VideoFiles);
			break;
		}
	}
}

// Called every frame
void AVideoPlayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVideoPlayerActor::LoadVideoFiles()
{
	if (VideoFilesPath.IsEmpty())
	{
		return;
	}

	//TArray<FString> VideoFiles;
	IFileManager::Get().FindFiles(VideoFiles, *(FPaths::ProjectDir() + VideoFilesPath));
	for (FString& VideoFile : VideoFiles)
	{
		VideoFile = FPaths::ProjectDir() + VideoFilesPath + "/" + VideoFile;
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *VideoFile);
	}
}

void AVideoPlayerActor::PlayMovie()
{
	if (MyMediaPlayer)
	{
		if (VideoFiles.Num() > 0)
		{
			MyMediaPlayer->OpenFile(VideoFiles[0]);
			GetWorldTimerManager().SetTimer(TempHandle, [&]() 
			{
				if (MyMediaPlayer->GetVideoTrackAspectRatio(0, 0) != 0.f)
				{
					float Ratio = MyMediaPlayer->GetVideoTrackAspectRatio(0, 0);
					UE_LOG(LogTemp, Error, TEXT("[%f]"), Ratio);
					GetWorldTimerManager().ClearTimer(TempHandle);
				}
			}, 0.05, true);
			GetWorldTimerManager().SetTimer(TempClearHandle, [&]()
			{
				GetWorldTimerManager().ClearTimer(TempHandle);
			}, 0.5, false);
		}
	}
}