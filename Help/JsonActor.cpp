// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonActor.h"
#include "Http.h"
#include "JsonUtilities.h"
#include "MyActor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Networking.h"
#include "UnrealClient.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/GameViewportClient.h"
#include "SWindow.h"
#include "Windows/WindowsHWrapper.h"
#include "Windows/COMPointer.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <shlobj.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include <string>


// Sets default values
AJsonActor::AJsonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AJsonActor::BeginPlay()
{
	Super::BeginPlay();

	FDisplayMetrics Display;
	FDisplayMetrics::GetDisplayMetrics(Display);
	DesktopResolution = FVector2D(Display.PrimaryDisplayWidth, Display.PrimaryDisplayHeight);

	TCPConnect();
}

// Called every frame
void AJsonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AJsonActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}
}

void AJsonActor::JsonCreate(AMyActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (Actor->URL_Create.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_Create"));
		return;
	}

	TSharedPtr<FJsonObject> NewRecord = MakeShareable(new FJsonObject());
	NewRecord->SetStringField("Flag", "3");
	NewRecord->SetStringField("BuildingCode", Actor->BuildingCode);
	NewRecord->SetStringField("CreatedAt", FDateTime::Now().ToIso8601());
	NewRecord->SetStringField("UpdatedAt", FDateTime::Now().ToIso8601());
	NewRecord->SetStringField("LastCheckTime", FDateTime::Now().ToIso8601());
	NewRecord->SetNumberField("ModelX", Actor->GetActorLocation().X);
	NewRecord->SetNumberField("ModelY", Actor->GetActorLocation().Y);

	FString JsonString;
	TSharedRef<TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(NewRecord.ToSharedRef(), JsonWriter);

	FString URL = BaseURL + Actor->URL_Create;

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetContentAsString(JsonString);
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnCreateResponseReceived);

	RAMap.Add(Request, Actor);

	Request->ProcessRequest();
}

void AJsonActor::OnCreateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Create failed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	FString JsonString = Response->GetContentAsString();
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to parse json=[%s]"), *JsonString);
		return;
	}

	FString Code = JsonObject->GetStringField("Code");

	AMyActor* MyActor = RAMap[Request];
	RAMap.Remove(Request);
	if (!MyActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is Invalid"));
		return;
	}
	MyActor->Code = Code;

	UE_LOG(LogTemp, Warning, TEXT("Actor with code [%s] created!"), *Code);
}

void AJsonActor::JsonDelete(AMyActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (Actor->Code.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Code"));
		return;
	}
	if (Actor->URL_Delete.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_Delete"));
		return;
	}

	FString URL = BaseURL + Actor->URL_Delete + Actor->Code;
	Actor->Destroy();

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnDeleteResponseReceived);
	Request->ProcessRequest();
}

void AJsonActor::OnDeleteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Delete failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Actor with code [%s] deleted!"), *(Response->GetContentAsString().TrimQuotes()));
}

void AJsonActor::JsonPreUpdate(AMyActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (Actor->Code.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Code"));
		return;
	}
	if (Actor->URL_Query.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_Query"));
		return;
	}
	if (Actor->URL_Update.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_Update"));
		return;
	}

	FString URL = BaseURL + Actor->URL_Query + Actor->Code;

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnPreUpdateResponseReceived);

	RAMap.Add(Request, Actor);

	Request->ProcessRequest();
}

void AJsonActor::OnPreUpdateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation PreUpdate failed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	FString JsonString = Response->GetContentAsString();
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to parse json=[%s]"), *JsonString);
		return;
	}

	AMyActor* MyActor = RAMap[Request];
	RAMap.Remove(Request);
	if (!MyActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is Invalid"));
		return;
	}

	JsonObject->SetStringField("Flag", "3");
	JsonObject->SetStringField("BuildingCode", MyActor->BuildingCode);
	JsonObject->SetStringField("UpdatedAt", FDateTime::Now().ToIso8601());
	JsonObject->SetNumberField("ModelX", MyActor->GetActorLocation().X);
	JsonObject->SetNumberField("ModelY", MyActor->GetActorLocation().Y);

	FString UpdatedContent;
	TSharedRef<TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&UpdatedContent);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	FString URL = BaseURL + MyActor->URL_Update;
	JsonUpdate(URL, UpdatedContent);
}

void AJsonActor::JsonUpdate(const FString& URL, const FString& UpdatedContent)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetContentAsString(UpdatedContent);
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnUpdateResponseReceived);
	Request->ProcessRequest();
}

void AJsonActor::OnUpdateResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Update failed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	FString JsonString = Response->GetContentAsString();
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to parse json=[%s]"), *JsonString);
		return;
	}

	FString Location;
	for (auto& ValueElem : JsonObject->GetArrayField("Location"))
	{
		Location = Location + ValueElem->AsString() + ", ";
	}
	Location.RemoveAt(Location.Len() - 2, 2);
	
	UE_LOG(LogTemp, Warning, TEXT("Actor Location2D[%s] updated!"), *(Location));
}

void AJsonActor::JsonQuery(AMyActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (Actor->Code.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Code"));
		return;
	}
	if (Actor->URL_Query.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_Query"));
		return;
	}

	FString URL = BaseURL + Actor->URL_Query + Actor->Code;

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnQueryResponseReceived);
	Request->ProcessRequest();
}

void AJsonActor::OnQueryResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Query failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s]"), *(Response->GetContentAsString()));
}

void AJsonActor::LoadMyActorsByBuildingCode(TSubclassOf<AMyActor> ActorClass, const FString& BuildingCode)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid ActorClass!"));
		return;
	}
	if (BuildingCode.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing BuildingCode"));
		return;
	}

	FString URL_ByBuildingCode = ActorClass.GetDefaultObject()->URL_ByBuildingCode;
	if (URL_ByBuildingCode.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing URL_ByBuildingCode"));
		return;
	}
	FString URL = BaseURL + URL_ByBuildingCode + BuildingCode;

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL(URL);
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnLoadResponseReceived);

	RCMap.Add(Request, ActorClass);

	Request->ProcessRequest();
}

void AJsonActor::OnLoadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	TSubclassOf<AMyActor> ActorClass = RCMap[Request];
	RCMap.Remove(Request);

	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation LoadByBuildingCode failed: %s"), *(ActorClass->GetName()));
		return;
	}

	FString JsonString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to parse json=[%s]"), *JsonString);
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* ResultArrayPtr = nullptr;
	if (!JsonObject->TryGetArrayField("Result", ResultArrayPtr))
	{
		UE_LOG(LogTemp, Error, TEXT("Json parse error: ResultArray"));
		return;
	}

	for (const TSharedPtr<FJsonValue>& ValueElem : *ResultArrayPtr)
	{
		const TSharedPtr<FJsonObject>* Record;
		if (!ValueElem->TryGetObject(Record))
		{
			UE_LOG(LogTemp, Error, TEXT("Json parse error: RecordElement"));
			continue;
		}

		FVector Location = FVector((*Record)->GetNumberField("ModelX"), (*Record)->GetNumberField("ModelY"), 5000);
		AMyActor* MyActor = GetWorld()->SpawnActor<AMyActor>(ActorClass, Location, FRotator(0));
		MyActor->Code = (*Record)->GetStringField("Code");
		MyActor->SnapToFloor();
	}
}

void AJsonActor::JsonTryDelete(AMyActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (!(Actor->Code.IsEmpty()))
	{
		JsonDelete(Actor);
		return;
	}

	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	
	GetWorldTimerManager().SetTimer(Handle_ClearAll,
		[&]() { GetWorldTimerManager().ClearTimer(Handle_CheckCode);
				GetWorldTimerManager().ClearTimer(Handle_ClearAll);
				if (Actor) Actor->Destroy(); },
		4.0, false);

	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AJsonActor::CheckActorCode, Actor);
	GetWorldTimerManager().SetTimer(Handle_CheckCode, TimerDel, 0.1, true);
}

void AJsonActor::CheckActorCode(AMyActor* Actor)
{
	if (!Actor)
	{
		GetWorldTimerManager().ClearTimer(Handle_CheckCode);
		UE_LOG(LogTemp, Error, TEXT("Invalid Actor!"));
		return;
	}
	if (!(Actor->Code.IsEmpty()))
	{
		GetWorldTimerManager().ClearTimer(Handle_CheckCode);
		JsonDelete(Actor);
		return;
	}
}

void AJsonActor::TCPConnect(const FString& SocketName, const FString& IP, const int32 Port)
{
	IPAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	IPAddr->SetIp(*IP, bIsValid);
	IPAddr->SetPort(Port);

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid IP Address: %s"), *IP);
		return;
	}

	SenderSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, SocketName, false);
	if (!SenderSocket->Connect(*IPAddr))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to connect to %s:%d"), *IP, Port);
	}
}

void AJsonActor::ShowUI(FVector2D Position, FVector2D Size, const FString& URL_UI)
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("No sender socket"));
		return;
	}
	if (SenderSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Error, TEXT("No connection"));
		return;
	}

	TSharedPtr<FJsonObject> Param = MakeShareable(new FJsonObject());;
	Param->SetNumberField("x", FMath::Min(Position.X, DesktopResolution.X - Size.X));
	Param->SetNumberField("y", FMath::Min(Position.Y, DesktopResolution.Y - Size.Y));
	Param->SetNumberField("width", Size.X);
	Param->SetNumberField("height", Size.Y);
	Param->SetStringField("url", URL_UI);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField("Command", "ShowWebviewWithPosition");
	JsonObject->SetObjectField("Parameter", Param);

	FString JsonString;
	TSharedRef<TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	int32 BytesSent = 0;
	SenderSocket->Send((uint8*)TCHAR_TO_ANSI(JsonString.GetCharArray().GetData()), JsonString.Len(), BytesSent);

	if (BytesSent <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to send data"));
	}
}

void AJsonActor::HideUI()
{
	if (!SenderSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("No sender socket"));
		return;
	}
	if (SenderSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
	{
		UE_LOG(LogTemp, Error, TEXT("No connection"));
		return;
	}

	FString JsonString = "{\"Command\": \"HideWebview\"}";
	int32 BytesSent = 0;
	SenderSocket->Send((uint8*)TCHAR_TO_ANSI(JsonString.GetCharArray().GetData()), JsonString.Len(), BytesSent);

	if (BytesSent <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to send data"));
	}
}

void AJsonActor::UploadFiles()
{
	FString PlanCode = *(GetWorld()->GetAuthGameMode()->GetClass()->FindPropertyByName("PlanCode")
		->ContainerPtrToValuePtr<FString>(GetWorld()->GetAuthGameMode()));

	if (PlanCode.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No PlanCode!"));
		return;
	}

	FString URL = PlanURL;
	FString ImageFolderPath = FPaths::GameDir() + "Saved/Screenshots/Windows/";
	TArray<FString> ImageNames;

	//TArray<FString> AllImageNames;
	//IFileManager::Get().FindFiles(AllImageNames, *ImageFolderPath, TEXT("png"));
	//if (AllImageNames.Num() == 0)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("No files in Folder %s"), *ImageFolderPath);
	//	return;
	//}

	TSharedPtr<FJsonObject> PlanJson = MakeShareable(new FJsonObject());
	PlanJson->SetStringField("PlanCode", PlanCode);
	PlanJson->SetStringField("Title", "PlaceHolder");

	TArray<TSharedPtr<FJsonValue>> PlanFilesArray;
	for (auto& PlanFile : PlanStruct.StepInfos)
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		JsonObject->SetStringField("SectionHeading", PlanFile.StepName);
		JsonObject->SetStringField("Description", PlanFile.StepDescription);
		JsonObject->SetStringField("FileName", "");

		TArray<TSharedPtr<FJsonValue>> StepsArray;
		for (auto& elem : PlanFile.SubSteps)
		{
			TSharedPtr<FJsonObject> InnerJsonObject = MakeShareable(new FJsonObject());
			InnerJsonObject->SetStringField("StepHeading", elem.SubStepDescription);
			InnerJsonObject->SetStringField("StepDescription", "");
			InnerJsonObject->SetStringField("StepFileName", elem.FileName + ".png");

			TSharedPtr<FJsonValue> InnerJsonValue = MakeShareable(new FJsonValueObject(InnerJsonObject));
			StepsArray.Add(InnerJsonValue);

			ImageNames.Add(elem.FileName + ".png");
		}

		JsonObject->SetArrayField("Steps", StepsArray);

		TSharedPtr<FJsonValue> JsonValue = MakeShareable(new FJsonValueObject(JsonObject));
		PlanFilesArray.Add(JsonValue);
	}

	PlanJson->SetArrayField("PlanFiles", PlanFilesArray);

	FString JsonString;
	TSharedRef<TJsonWriter<> > JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(PlanJson.ToSharedRef(), JsonWriter);

	FString Boundary = "HelloWorld" + FString::FromInt(FDateTime::Now().GetTicks());
	FString BeginBoundary = "\r\n--" + Boundary + "\r\n";
	FString EndBoundary = "\r\n--" + Boundary + "--\r\n";
	FString FileHeader = "Content-Disposition: form-data; name=\"Files\"; filename=\"ReplaceImageName\"\r\nContent-Type: image/png\r\n\r\n";

	TArray<uint8> UploadContent;

	UploadContent.Append((uint8*)TCHAR_TO_UTF8(*BeginBoundary), FTCHARToUTF8_Convert::ConvertedLength(*BeginBoundary, BeginBoundary.Len()));
	FString JsonHeader("Content-Disposition: form-data; name=\"Description\"\r\n\r\n");
	UploadContent.Append((uint8*)TCHAR_TO_UTF8(*JsonHeader), FTCHARToUTF8_Convert::ConvertedLength(*JsonHeader, JsonHeader.Len()));
	UploadContent.Append((uint8*)TCHAR_TO_UTF8(*JsonString), FTCHARToUTF8_Convert::ConvertedLength(*JsonString, JsonString.Len()));
	
	for (const FString& FileName : ImageNames)
	{
		TArray<uint8> BinaryData;
		if (FFileHelper::LoadFileToArray(BinaryData, *(ImageFolderPath + FileName)))
		{
			UploadContent.Append((uint8*)TCHAR_TO_UTF8(*BeginBoundary), FTCHARToUTF8_Convert::ConvertedLength(*BeginBoundary, BeginBoundary.Len()));
			FString ImageHeader = FileHeader.Replace(TEXT("ReplaceImageName"), *FileName);
			UploadContent.Append((uint8*)TCHAR_TO_UTF8(*ImageHeader), FTCHARToUTF8_Convert::ConvertedLength(*ImageHeader, ImageHeader.Len()));
			UploadContent.Append(BinaryData);
		}
	}
	UploadContent.Append((uint8*)TCHAR_TO_UTF8(*EndBoundary), FTCHARToUTF8_Convert::ConvertedLength(*EndBoundary, EndBoundary.Len()));

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Cookie", Cookie);
	Request->SetHeader("Content-Type", "multipart/form-data; boundary=" + Boundary);
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetContent(UploadContent);
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnUploadResponseReceived);
	Request->ProcessRequest();
	//FFileHelper::SaveArrayToFile(UploadContent, *(FPaths::GameDir() + "Saved/Upload.docx"));
}

void AJsonActor::OnUploadResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
	if (!bSuccess || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Operation Upload failed"));
		return;
	}
	TArray<uint8> DownloadContent = Response->GetContent();

	bool bOpenSuccess = false;
	TArray<FString> OutFilenames;

	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		FileDialog->SetTitle(TEXT("保存预案"));
		FString FileType = "Word(*.docx)|*.docx";
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			FileType.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());
		const void* ParentWindowHandle = GetWorld()->GetGameViewport()->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		if (SUCCEEDED(FileDialog->Show((HWND)ParentWindowHandle)))
		{
			int32 OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1;
			}
			auto AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames[OutFilenames.Add(InFilename)];
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};
			
			TComPtr<IShellItem> Result;
			if (SUCCEEDED(FileDialog->GetResult(&Result)))
			{
				PWSTR pFilePath = nullptr;
				if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
				{
					bOpenSuccess = true;

					FString SaveFilePath = pFilePath;
					if (FileDialogFilters.IsValidIndex(OutFilterIndex))
					{
						const FString ExtensionPattern = FileDialogFilters[OutFilterIndex].pszSpec;
						TArray<FString> SaveExtensions;
						ExtensionPattern.ParseIntoArray(SaveExtensions, TEXT(";"));
						FString CleanExtension = SaveExtensions[0];
						if (CleanExtension == TEXT("*.*"))
						{
							CleanExtension.Reset();
						}
						else
						{
							int32 WildCardIndex = INDEX_NONE;
							if (CleanExtension.FindChar(TEXT('*'), WildCardIndex))
							{
								CleanExtension = CleanExtension.RightChop(WildCardIndex + 1);
							}
						}
						FString SaveFileName = FPaths::GetCleanFilename(SaveFilePath);
						SaveFilePath = FPaths::GetPath(SaveFilePath);
						if (FPaths::GetExtension(SaveFileName).IsEmpty() && !CleanExtension.IsEmpty())
						{
							SaveFileName = FPaths::SetExtension(SaveFileName, CleanExtension);
						}
						SaveFilePath /= SaveFileName;
					}
					AddOutFilename(SaveFilePath);

					::CoTaskMemFree(pFilePath);
				}
			}
		}
	}

	if (bOpenSuccess)
		FFileHelper::SaveArrayToFile(DownloadContent, *(OutFilenames[0]));
	else
		UE_LOG(LogTemp, Error, TEXT("Fail to save plan"));
}

void AJsonActor::TakeScreenShot(FString Filename, bool bShowUI)
{
	FScreenshotRequest::RequestScreenshot(Filename, bShowUI, false);
}


FSubStepInfo AJsonActor::MakeSubStep(FString SubStepDescription, FString ScreenShotFileName)
{
	FSubStepInfo Temp;
	Temp.FileName = ScreenShotFileName;
	Temp.SubStepDescription = SubStepDescription;
	return Temp;
}

FStepInfo AJsonActor::MakeStep(FString StepName, FString StepDescription, TArray<FSubStepInfo>& InSubSteps)
{
	FStepInfo Temp;
	Temp.StepDescription = StepDescription;
	Temp.StepName = StepName;
	return Temp;
}


int32 AJsonActor::AddStep(FString StepName, FString StepDescription)
{
	FStepInfo Temp;
	Temp.StepDescription = StepDescription;
	Temp.StepName = StepName;
	return PlanStruct.StepInfos.Add(Temp);
}

void AJsonActor::AddSubStep(FString InStepName, FString InSubStepText, FString InFileName)
{
	for (FStepInfo elem : PlanStruct.StepInfos)
	{
		if (elem.StepName == InStepName)
		{
			elem.SubSteps.Add(MakeSubStep(InSubStepText, InFileName));
		}
	}
}

void AJsonActor::ClearPlan()
{
	this->PlanStruct.StepInfos.Empty();
}

void AJsonActor::TestCase()
{
	//FSubStepInfo BB;
	//BB.FileName = "ScreenShot";
	//BB.SubStepDescription = TEXT("你好");
	//FStepInfo AA;
	//AA.StepName = TEXT("吃了吗");
	//AA.StepDescription = "吃饱了";
	//AA.SubSteps.Add(BB);
	//PlanStruct.StepInfos.Add(AA);
	//FString* PlanCodePtr = GetWorld()->GetAuthGameMode()->GetClass()->FindPropertyByName("PlanCode")
	//	->ContainerPtrToValuePtr<FString>(GetWorld()->GetAuthGameMode());
	//*PlanCodePtr = "0fc1466b-b29e-466b-8013-6cc41c8662f9";

	//TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	//Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	//Request->SetHeader("Content-Type", "application/json");
	//Request->SetHeader("Cookie", Cookie);
	//Request->SetURL("http://xiaofang.******.com:8888/api/room_mark_by_building_code?code=1cd18c59-f5cd-43fc-aae4-66e77e400151");
	//Request->SetVerb("GET");
	//Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnQueryResponseReceived);
	//Request->ProcessRequest();

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetURL("http://xiaofang.******.com:8888/setup.exe");
	Request->SetVerb("GET");
	Request->OnProcessRequestComplete().BindUObject(this, &AJsonActor::OnUploadResponseReceived);
	Request->ProcessRequest();
}