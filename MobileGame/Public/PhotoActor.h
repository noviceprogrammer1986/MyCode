// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TcpSender.h"
#include "PhotoActor.generated.h"


class FTcpSender;
class UMediaPlayer;
class FAndroidCameraPlayer;

UCLASS()
class MOBILEGAME_API APhotoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhotoActor();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Tcp")
		void StartTcpSender();

	UFUNCTION(BlueprintCallable, Category = "Photo")
		bool OpenCamera();

	UFUNCTION(BlueprintCallable, Category = "Photo")
		void TakePicture(float Delay = -1.f);

	UFUNCTION(BlueprintCallable, Category = "Photo")
		void ResumePreview();

	UFUNCTION(BlueprintCallable, Category = "Photo")
		void CloseCamera();

	UFUNCTION(BlueprintCallable, Category = "Tcp")
		void SetServerIP(const FString& InServerIP);

	UFUNCTION(BlueprintCallable, Category = "Photo")
		bool OpenCameraFor(int32 CurrentPoint);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Photo")
		FString MediaPlayerRef;
	UPROPERTY(EditAnywhere, Category = "Tcp")
		FString ServerIP;
	UPROPERTY(EditAnywhere, Category = "Tcp")
		int32 Port;
	UPROPERTY(EditAnywhere, Category = "Photo")
		FString ImageDirectory;
	UPROPERTY(EditAnywhere, Category = "Photo")
		FString ImageExtension;
	UPROPERTY(EditAnywhere, Category = "Photo")
		int32 CameraWidth = 1280;
	UPROPERTY(EditAnywhere, Category = "Photo")
		int32 CameraHeight = 720;
	UPROPERTY(EditAnywhere, Category = "Photo")
		int32 CameraFps = 30;
	UPROPERTY(EditAnywhere, Category = "Photo")
		int32 PhotoWidth = 1920;
	UPROPERTY(EditAnywhere, Category = "Photo")
		int32 PhotoHeight = 1080;

	UPROPERTY()
		FSenderMiscData CurrentMiscData;

private:
	TSharedPtr<FTcpSender> TcpSender;
	UPROPERTY()
		UMediaPlayer* MyUPlayer;
	FAndroidCameraPlayer* MyIPlayer;
	UPROPERTY()
		FString CamUrl;
	FTimerHandle TempHandle;
};
