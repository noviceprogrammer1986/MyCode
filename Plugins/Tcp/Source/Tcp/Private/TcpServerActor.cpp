// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpServerActor.h"
#include "TcpReceiver.h"
#include "PhotoListWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObjectIterator.h"
#include "EngineUtils.h"

#include "PatrolPointActor.h"


ATcpServerActor::ATcpServerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ATcpServerActor::BeginPlay()
{
	Super::BeginPlay();

	StartTcpListener();
}

void ATcpServerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TcpListener.IsValid())
	{
		TcpListener->Stop();
	}
	for (const auto& TcpReceiver : TcpReceivers)
	{
		if (TcpReceiver.IsValid())
		{
			TcpReceiver->Stop();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ATcpServerActor::StartTcpListener()
{
	if (TcpListener.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("TcpListener is already running!"));
		return;
	}

	TSharedRef<FInternetAddr> LocalAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr(0, Port);
	TcpListener = MakeShareable(new FTcpListener(FIPv4Endpoint(LocalAddress)));
	TcpListener->OnConnectionAccepted().BindUObject(this, &ATcpServerActor::OnConnectionAccepted);
}

bool ATcpServerActor::OnConnectionAccepted(FSocket* Socket, const FIPv4Endpoint& Endpoint)
{
	TSharedPtr<FTcpReceiver> TcpReceiver = MakeShareable(new FTcpReceiver(this, "TcpReceiver" + FString::FromInt(TcpReceivers.Num()), Socket, SaveDirectory, FileExtension));
	TcpReceivers.Add(TcpReceiver);
	UE_LOG(LogTemp, Warning, TEXT("Accept connection from [%s:%d]"), *Endpoint.Address.ToString(), Endpoint.Port);
	return true;
}

void ATcpServerActor::OnTextureReady(UTexture2D* InTexture)
{
	Textures.Add(InTexture);

	if (PhotoListWidget)
	{
		PhotoListWidget->AddImage(InTexture);
	}
	else
	{
		TObjectIterator<UPhotoListWidget> WidgetItr;
		if (WidgetItr)
		{
			PhotoListWidget = *WidgetItr;
		}
		else
		{
			PhotoListWidget = CreateWidget<UPhotoListWidget>(GetWorld(), PhotoListWidgetClass);
			PhotoListWidget->AddToViewport();
		}
		for (const auto& NewTexture : Textures)
		{
			PhotoListWidget->AddImage(NewTexture);
		}
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
	}
}

void ATcpServerActor::OnTextureReadyFor(UTexture2D* InTexture, int32 CurrentPoint)
{
	for (TActorIterator<APatrolPointActor> Itr(GetWorld()); Itr; ++Itr)
	{
		if ((*Itr)->PointNum == CurrentPoint)
		{
			(*Itr)->SetTexture(InTexture);
			break;
		}
	}

	Textures.Add(InTexture);
	if (PhotoListWidget)
	{
		PhotoListWidget->AddImage(InTexture);
	}
	else
	{
		for (TObjectIterator<UPhotoListWidget> WidgetItr; WidgetItr; ++WidgetItr)
		{
			if ((*WidgetItr)->GetWorld() == GetWorld())
			{
				PhotoListWidget = *WidgetItr;
			}
		}
		if (PhotoListWidget == nullptr)
		{
			PhotoListWidget = CreateWidget<UPhotoListWidget>(GetWorld(), PhotoListWidgetClass);
			PhotoListWidget->AddToViewport();
		}
		for (const auto& NewTexture : Textures)
		{
			PhotoListWidget->AddImage(NewTexture);
		}
	}
}