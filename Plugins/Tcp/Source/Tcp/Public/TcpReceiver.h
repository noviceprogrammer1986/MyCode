// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"
#include "TcpReceiver.generated.h"


USTRUCT()
struct FReceiverMiscData
{
	GENERATED_BODY()

	UPROPERTY()
		int32 CurrentPoint = 0;
};

class FSocket;
class UTexture2D;
class ATcpServerActor;

class TCP_API FTcpReceiver : public FRunnable
{
public:
	FTcpReceiver(ATcpServerActor* InOwner, FString ThreadName, FSocket* InSocket, FString InDirectory, FString InFileExtension);
	~FTcpReceiver();

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

	UTexture2D* CurrentTexture = nullptr;

private:
	FRunnableThread* Thread;
	FSocket* ConnectionSocket;
	FString SaveDirectory;
	FString FileExtension;
	bool Stopping = false;
	ATcpServerActor* Owner;

	bool LoadImage(const TArray<uint8>& ImageData, const FString& Filename);
};
