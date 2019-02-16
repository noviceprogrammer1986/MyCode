// Fill out your copyright notice in the Description page of Project Settings.

#include "PhotoActor.h"
#include "MediaPlayer.h"
#include "MediaPlayerFacade.h"
#include "MediaCaptureSupport.h"
#include "TimerManager.h"

#if PLATFORM_ANDROID
#include "AndroidCamera/Private/Player/AndroidCameraPlayer.h"
#endif


// Sets default values
APhotoActor::APhotoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APhotoActor::BeginPlay()
{
	Super::BeginPlay();

}

void APhotoActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TcpSender.IsValid())
	{
		TcpSender->Stop();
	}
	Super::EndPlay(EndPlayReason);
}

void APhotoActor::StartTcpSender()
{
	if (TcpSender.IsValid())
	{
		TcpSender->Stop();
		TcpSender.Reset();
	}
	TcpSender = MakeShareable(new FTcpSender("TcpSender", ServerIP, Port, ImageDirectory, ImageExtension, CurrentMiscData));
}

bool APhotoActor::OpenCamera()
{
	if (!MyUPlayer)
	{
		MyUPlayer = LoadObject<UMediaPlayer>(this, *MediaPlayerRef);
		if (!MyUPlayer)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load MediaPlayer"));
			return false;
		}
	}

	if (CamUrl.IsEmpty())
	{
		TArray<FMediaCaptureDeviceInfo> DeviceInfos;
		MediaCaptureSupport::EnumerateVideoCaptureDevices(DeviceInfos);

		for (const auto& DeviceInfo : DeviceInfos)
		{
			if (DeviceInfo.Type == EMediaCaptureDeviceType::WebcamRear)
			{
				CamUrl = DeviceInfo.Url + FString::Printf(TEXT("?width=%d?height=%d?fps=%d"), CameraWidth, CameraHeight, CameraFps);
				break;
			}
		}
	}

	return MyUPlayer->OpenUrl(CamUrl);
}

void APhotoActor::TakePicture(float Delay)
{
#if PLATFORM_ANDROID
	if (MyUPlayer && MyUPlayer->GetPlayerName() == FName("AndroidCamera"))
	{
		if (!MyIPlayer)
		{
			MyIPlayer = reinterpret_cast<FAndroidCameraPlayer*>(MyUPlayer->GetPlayerFacade()->GetPlayer().Get());
		}

		FString ImageFullname = ImageDirectory + "/" + FString::FromInt(CurrentMiscData.CurrentPoint) + "/" + FDateTime::Now().ToString() + "." + ImageExtension;
		if (MyIPlayer && MyIPlayer->JavaCameraPlayer->TakePicture(ImageFullname, PhotoWidth, PhotoHeight))
		{
			if (Delay >= 0)
			{
				GetWorldTimerManager().SetTimer(TempHandle, [&] { ResumePreview(); }, Delay, false);
			}
		}
	}
#endif
}

void APhotoActor::ResumePreview()
{
#if PLATFORM_ANDROID
	GetWorldTimerManager().ClearTimer(TempHandle);

	if (MyUPlayer)
	{
		MyUPlayer->OpenUrl(CamUrl);
	}
#endif
}

void APhotoActor::CloseCamera()
{
	if (MyUPlayer)
	{
		MyUPlayer->Close();
	}

	CurrentMiscData.CurrentPoint = 0;
}

void APhotoActor::SetServerIP(const FString& InServerIP)
{
	ServerIP = InServerIP;
}

bool APhotoActor::OpenCameraFor(int32 CurrentPoint)
{
	CurrentMiscData.CurrentPoint = CurrentPoint;
	return OpenCamera();
}
