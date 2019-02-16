// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeechRecognizer.h"
#include "MyActor.h"
#include "Async.h"
#include "Paths.h"

#include "AllowWindowsPlatformTypes.h"
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 4996)
#ifndef DeleteFile
#define DeleteFile DeleteFileW
#endif
#ifndef MoveFile
#define MoveFile MoveFileW
#endif
#ifndef LoadString
#define LoadString LoadStringW
#endif
#ifndef InterlockedDecrement
#define InterlockedDecrement _InterlockedDecrement
#endif
#ifndef InterlockedIncrement
#define InterlockedIncrement _InterlockedIncrement
#endif
#ifndef GetMessage
#define GetMessage GetMessageW
#endif

#include <sapi.h>
#include <sphelper.h>
#include <atlbase.h>

#undef DeleteFile
#undef MoveFile
#undef LoadString
#undef InterlockedDecrement
#undef InterlockedIncrement
#undef GetMessage
#pragma warning(pop)
#include "HideWindowsPlatformTypes.h"


SpeechRecognizer::SpeechRecognizer(AMyActor* InOwner)
{
	Owner = InOwner;
	Thread = FRunnableThread::Create(this, TEXT("SpeechRecognizer"));
}

SpeechRecognizer::~SpeechRecognizer()
{
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

bool SpeechRecognizer::Init()
{
	return true;
}

inline HRESULT BlockForResult(ISpRecoContext* pRecoCtxt, ISpRecoResult** ppResult)
{
	HRESULT hr = S_OK;
	CSpEvent event;

	while (SUCCEEDED(hr) &&
		SUCCEEDED(hr = event.GetFrom(pRecoCtxt)) &&
		hr == S_FALSE)
	{
		hr = pRecoCtxt->WaitForNotifyEvent(INFINITE);
	}

	*ppResult = event.RecoResult();
	if (*ppResult)
	{
		(*ppResult)->AddRef();
	}

	return hr;
}

uint32 SpeechRecognizer::Run()
{
	if (SUCCEEDED(::CoInitialize(NULL)))
	{
		CComPtr<ISpRecoContext> cpRecoCtxt;
		CComPtr<ISpRecoGrammar> cpGrammar;
		CComPtr<ISpRecognizer> cpRecognizer;
		CComPtr<ISpAudio> m_pAudio;
		cpRecoCtxt.CoCreateInstance(CLSID_SpInProcRecoContext);

		cpRecoCtxt->GetRecognizer(&cpRecognizer);
		SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &m_pAudio);
		cpRecognizer->SetInput(m_pAudio, true);

		cpRecognizer->SetRecoState(SPRST_ACTIVE);

		cpRecoCtxt->SetNotifyWin32Event();
		cpRecoCtxt->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
		cpRecoCtxt->SetAudioOptions(SPAO_NONE, NULL, NULL);
		cpRecoCtxt->CreateGrammar(0, &cpGrammar);
		//cpGrammar->LoadDictation(NULL, SPLO_STATIC);
		//cpGrammar->SetDictationState(SPRS_ACTIVE);
		cpGrammar->LoadCmdFromFile(*(FPaths::ProjectDir() + "MyGrammer.xml"), SPLO_STATIC);
		cpGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);

		USES_CONVERSION;

		const WCHAR* const pchStop = L"退出语音识别";

		while (!bStopping)
		{
			//cpGrammar->SetDictationState(SPRS_ACTIVE);
			cpGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
			CSpDynamicString dstrText;
			CSpEvent event;
			while (event.GetFrom(cpRecoCtxt) == S_FALSE)
			{
				if (bStopping)
				{
					::CoUninitialize();
					return 0;
				}
				cpRecoCtxt->WaitForNotifyEvent(100);
			}
			CComPtr<ISpRecoResult> cpResult = event.RecoResult();

			//cpGrammar->SetDictationState(SPRS_INACTIVE);
			cpGrammar->SetRuleState(NULL, NULL, SPRS_INACTIVE);

			if (SUCCEEDED(cpResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE,
				true, &dstrText, NULL)))
			{
				AsyncTask(ENamedThreads::GameThread, [=]() 
				{
					Owner->OnSpeechRecognized(dstrText.m_psz);
				});

				cpResult.Release();
			}
			if (_wcsicmp(dstrText, pchStop) == 0)
			{
				break;
			}
		}
	}
	::CoUninitialize();
	return 0;
}

void SpeechRecognizer::Stop()
{
	bStopping = true;
}

void SpeechRecognizer::Exit()
{

}
