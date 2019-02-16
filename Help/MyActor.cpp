// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"
//#include "AllowWindowsPlatformTypes.h"
//#include <sapi.h>
//#include "HideWindowsPlatformTypes.h"
//#include "Async.h"
#include "SpeechRecognizer.h"
#include "Engine/Engine.h"


// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
	MySpeechRecognizer = MakeShareable(new SpeechRecognizer(this));
	//Async<void>(EAsyncExecution::TaskGraph, [&]() 
	//{
	//	ISpVoice* pVoice = NULL;
	//	if (FAILED(::CoInitialize(NULL)))
	//		return;
	//	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pVoice->Speak(L"你好", 0, NULL);
	//		pVoice->Release();
	//		pVoice = NULL;
	//	}
	//	::CoUninitialize();
	//});
}

void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MySpeechRecognizer.IsValid())
	{
		MySpeechRecognizer->Stop();
	}
	Super::EndPlay(EndPlayReason);
}

void AMyActor::OnSpeechRecognized(FString Speech)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, Speech);
}