// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpSender.h"
#include "Networking.h"
#include "FileManager.h"
#include "FileHelper.h"
#include "BufferArchive.h"
#include "Engine/Engine.h"


FArchive& operator<<(FArchive &Ar, FSenderMiscData& MiscData)
{
	Ar << MiscData.CurrentPoint;

	return Ar;
}

FTcpSender::FTcpSender(FString ThreadName, FString InServerIP, int32 InPort, FString InDirectory, FString InFileExtension, FSenderMiscData InMiscData)
{
	if (InMiscData.CurrentPoint > 0)
	{
		ServerIP = InServerIP;
		Port = InPort;
		ImageDirectory = InDirectory + "/" + FString::FromInt(InMiscData.CurrentPoint);
		FileExtension = InFileExtension;
		MiscData = InMiscData;

		Thread = FRunnableThread::Create(this, *ThreadName);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("Invalid misc data!"));
	}
}

FTcpSender::~FTcpSender()
{
	ImageDataArray.Empty();

	if (Thread != nullptr)
	{
		Thread->Kill(false);
		delete Thread;
	}
	if (Socket != nullptr)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}

bool FTcpSender::Init()
{
	return true;
}

uint32 FTcpSender::Run()
{
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid = false;
	RemoteAddress->SetIp(*ServerIP, bIsValid);
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("InValid IP [%s]"), *ServerIP);
		return 0;
	}
	RemoteAddress->SetPort(Port);
	Socket = FTcpSocketBuilder("SenderSocket");
	if (Socket == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("failed to build socket!"));
		return 0;
	}

	float Timeout = 3.f;
	while (!Socket->Connect(*RemoteAddress))
	{
		if (Stopping)
			return 0;
		FPlatformProcess::Sleep(1);
		Timeout -= 1.f;
		if (Timeout < 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("failed to connect to server!"));
			return 0;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Green, FString("Connected!"));

	TArray<FString> ImageFiles;
	IFileManager::Get().FindFiles(ImageFiles, *ImageDirectory, *FileExtension);
	if (ImageFiles.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString::Printf(TEXT("[%s] has no [%s] files!"), *ImageDirectory, *FileExtension));
		//UE_LOG(LogTemp, Error, TEXT("[%s] has no [%s] files!"), *ImageDirectory, *FileExtension);
		return 0;
	}

	//for (int32 i = 0; i < ImageFiles.Num(); ++i)
	//{
	//	TArray<uint8> ImageData;
	//	FFileHelper::LoadFileToArray(ImageData, *(ImageDirectory + "/" + ImageFiles[i]));
	//	ImageDataArray.Add(ImageData);
	//}

	//FString LastImageFile = ImageFiles[0];
	//FDateTime LastDateTime = IFileManager::Get().GetTimeStamp(*(ImageDirectory + "/" + LastImageFile));
	//for (int32 i = 1; i < ImageFiles.Num(); ++i)
	//{
	//	FDateTime TempDateTime = IFileManager::Get().GetTimeStamp(*(ImageDirectory + "/" + ImageFiles[i]));
	//	if (TempDateTime > LastDateTime)
	//	{
	//		LastDateTime = TempDateTime;
	//		LastImageFile = ImageFiles[i];
	//	}
	//}

	FString LastImageFile = ImageFiles[0];
	FDateTime LastDateTime;
	FDateTime::Parse(LastImageFile.LeftChop(FileExtension.Len() + 1), LastDateTime);
	for (int32 i = 1; i < ImageFiles.Num(); ++i)
	{
		FDateTime TempDateTime;
		FDateTime::Parse(ImageFiles[i].LeftChop(FileExtension.Len() + 1), TempDateTime);
		if (TempDateTime > LastDateTime)
		{
			LastDateTime = TempDateTime;
			LastImageFile = ImageFiles[i];
		}
	}

	TArray<uint8> TempImageData;
	FFileHelper::LoadFileToArray(TempImageData, *(ImageDirectory + "/" + LastImageFile));
	ImageDataArray.Add(TempImageData);


	FBufferArchive Header;
	int32 NumofImages = ImageDataArray.Num();
	Header << NumofImages;
	for (const auto& ImageData : ImageDataArray)
	{
		int32 Num = ImageData.Num();
		Header << Num;
	}
	Header << MiscData;

	FPlatformProcess::Sleep(1);

	int32 HeaderBytesSent = 0;
	if (Socket == nullptr || !Socket->Send(Header.GetData(), Header.Num(), HeaderBytesSent))
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("failed to send header!"));
		//UE_LOG(LogTemp, Error, TEXT("failed to send header!"));
		return 0;
	}

	uint32 DataSize;
	Timeout = 3.f;
	int32 ReceivedData = 0;
	int32 TempBytesReceived = 0;
	while (!Socket->HasPendingData(DataSize))
	{
		if (Stopping)
			return 0;
		FPlatformProcess::Sleep(0.05);
		Timeout -= 0.05;
		if (Timeout < 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("failed to receive header reply!"));
			return 0;
		}
	}
	Socket->Recv(reinterpret_cast<uint8*>(&ReceivedData), 4, TempBytesReceived);
	if (ReceivedData == NumofImages)
	{
		UE_LOG(LogTemp, Warning, TEXT("OK to send!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0, FColor::Red, FString("something wrong with header!"));
		//UE_LOG(LogTemp, Error, TEXT("something wrong with header!"));
		return 0;
	}

	int32 TotalBytesSent = 0;
	for (const auto& ImageData : ImageDataArray)
	{
		int32 Num = ImageData.Num();
		const uint8* Buffer = ImageData.GetData();
		while (Num > 0)
		{
			if (Stopping)
				return 0;

			if (Socket == nullptr || Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
			{
				UE_LOG(LogTemp, Error, TEXT("socket closed while sending!"));
				return 0;
			}

			int32 BytesSent = 0;
			if (!Socket->Send(Buffer, Num, BytesSent))
			{
				UE_LOG(LogTemp, Error, TEXT("something wrong happened while sending!"));
				FPlatformProcess::Sleep(0.5);
				continue;
			}

			Num -= BytesSent;
			Buffer += BytesSent;
			TotalBytesSent += BytesSent;
		}
		//FPlatformProcess::Sleep(0.1);
	}

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, FString::Printf(TEXT("All files have been sent! TotalBytes: [%d]"), TotalBytesSent));
	//UE_LOG(LogTemp, Warning, TEXT("All files have been sent! TotalBytes: [%d]"), TotalBytesSent);
	return 0;
}

void FTcpSender::Stop()
{
	Stopping = true;
}

void FTcpSender::Exit()
{

}