// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"
//#include "SpeechRecognizer.generated.h"


class AMyActor;

class TOOLS_API SpeechRecognizer : public FRunnable
{
public:
	SpeechRecognizer(AMyActor* InOwner);
	~SpeechRecognizer();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	FRunnableThread* Thread;
	AMyActor* Owner;
	bool bStopping = false;
};
