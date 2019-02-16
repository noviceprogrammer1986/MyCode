// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"
#include "TcpSender.generated.h"


USTRUCT()
struct FSenderMiscData
{
	GENERATED_BODY()

	UPROPERTY()
		int32 CurrentPoint = 0;
};

class FSocket;

class MOBILEGAME_API FTcpSender : public FRunnable
{
public:
	FTcpSender(FString ThreadName, FString InServerIP, int32 InPort, FString InDirectory, FString InFileExtension, FSenderMiscData InMiscData);
	~FTcpSender();

public:
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	FRunnableThread* Thread = nullptr;
	FSocket* Socket = nullptr;
	FString ServerIP;
	int32 Port;
	FString ImageDirectory;
	FString FileExtension;
	TArray<TArray<uint8>> ImageDataArray;
	bool Stopping = false;
	FSenderMiscData MiscData;
};
