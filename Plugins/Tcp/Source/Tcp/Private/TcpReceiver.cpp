// Fill out your copyright notice in the Description page of Project Settings.

#include "TcpReceiver.h"
#include "Networking.h"
#include "ArrayReader.h"
#include "FileHelper.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "RenderUtils.h"
#include "Async.h"
#include "Paths.h"

#include "TcpServerActor.h"


//static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");

FArrayReader& operator<<(FArrayReader &Ar, FReceiverMiscData& MiscData)
{
	Ar << MiscData.CurrentPoint;

	return Ar;
}

FTcpReceiver::FTcpReceiver(ATcpServerActor* InOwner, FString ThreadName, FSocket* InSocket, FString InDirectory, FString InFileExtension)
{
	ConnectionSocket = InSocket;
	SaveDirectory = FPaths::ProjectDir() + InDirectory;
	FileExtension = InFileExtension;
	Owner = InOwner;

	Thread = FRunnableThread::Create(this, *ThreadName);
}

FTcpReceiver::~FTcpReceiver()
{
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
	if (ConnectionSocket != nullptr)
	{
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
	}
}

bool FTcpReceiver::Init()
{
	return true;
}

uint32 FTcpReceiver::Run()
{
	uint32 DataSize;
	float Timeout = 3.f;
	while (!ConnectionSocket->HasPendingData(DataSize))
	{
		if (Stopping)
			return 0;

		FPlatformProcess::Sleep(0.05);
		Timeout -= 0.05;
		if (Timeout < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No header received!"));
			return 0;
		}
	}

	FArrayReader Header;
	Header.AddZeroed(4000);
	int32 NumofImages = 0;
	TArray<int32> ImageDataSizes;
	FReceiverMiscData MiscData;

	int32 TempBytesReceived = 0;
	ConnectionSocket->Recv(Header.GetData(), 4000, TempBytesReceived);

	Header << NumofImages;
	if (TempBytesReceived <= 0 || NumofImages > 999)
	{
		UE_LOG(LogTemp, Error, TEXT("server failed to read ImageNum!"));
		return 0;
	}
	for (int32 i = 0; i < NumofImages; ++i)
	{
		int32 ImageDataSize;
		Header << ImageDataSize;
		ImageDataSizes.Add(ImageDataSize);
	}
	Header << MiscData;

	UE_LOG(LogTemp, Warning, TEXT("Header received!"));

	int32 TempBytesSent;
	if (ConnectionSocket == nullptr || !ConnectionSocket->Send(reinterpret_cast<uint8*>(&NumofImages), 4, TempBytesSent))
	{
		UE_LOG(LogTemp, Error, TEXT("failed to send reply!"));
		return 0;
	}

	int32 TotalBytesSent = 0;
	for (int32 i = 0; i < ImageDataSizes.Num(); ++i)
	{
		int32 Size = ImageDataSizes[i];
		TArray<uint8> ImageData;
		ImageData.AddZeroed(Size);
		uint8* Buffer = ImageData.GetData();

		while (Size > 0)
		{
			if (Stopping)
				return 0;

			if (ConnectionSocket == nullptr || ConnectionSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
			{
				UE_LOG(LogTemp, Error, TEXT("socket closed while receiving!"));
				return 0;
			}

			if (!ConnectionSocket->HasPendingData(DataSize))
			{
				FPlatformProcess::Sleep(0.05);
				continue;
			}

			int32 BytesRead = 0;
			if (!ConnectionSocket->Recv(Buffer, Size, BytesRead))
			{
				UE_LOG(LogTemp, Error, TEXT("something wrong happened while receiving!"));
				FPlatformProcess::Sleep(0.5);
				continue;
			}

			Buffer += BytesRead;
			Size -= BytesRead;
			TotalBytesSent += BytesRead;
		}
		//Save
		FString Filename = FDateTime::Now().ToString() + "(" + FString::FromInt(i) + ")" + "." + FileExtension;
		if (FFileHelper::SaveArrayToFile(ImageData, *(SaveDirectory + "/" + Filename)))
		{
			UE_LOG(LogTemp, Warning, TEXT("File received: %s"), *Filename);
		}
		//Texture
		if (LoadImage(ImageData, Filename))
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				Owner->OnTextureReadyFor(CurrentTexture, MiscData.CurrentPoint);
			});
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("All files have been received! TotalBytes: [%d]"), TotalBytesSent);
	return 0;
}

void FTcpReceiver::Stop()
{
	Stopping = true;
}

void FTcpReceiver::Exit()
{
	
}

bool FTcpReceiver::LoadImage(const TArray<uint8>& ImageData, const FString& Filename)
{
	EImageFormat ImageFormat = Owner->ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
	if (ImageFormat == EImageFormat::Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("Unrecognized image file format: %s"), *Filename);
		return false;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = Owner->ImageWrapperModule.CreateImageWrapper(ImageFormat);
	if (!ImageWrapper.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create image wrapper for file: %s"), *Filename);
		return false;
	}

	const TArray<uint8>* RawData = nullptr;
	ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num());
	ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData);
	if (RawData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to decompress image file: %s"), *Filename);
		return false;
	}

	int32 InSizeX = ImageWrapper->GetWidth();
	int32 InSizeY = ImageWrapper->GetHeight();
	EPixelFormat InFormat = EPixelFormat::PF_B8G8R8A8;
	FName TextureName = FName(*("Texture_" + FPaths::GetBaseFilename(Filename)));

	CurrentTexture = nullptr;
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UTexture2D* NewTexture = NewObject<UTexture2D>(Owner, TextureName, RF_Transient);
		this->CurrentTexture = NewTexture;
	});

	float Timeout = 1.f;
	while (CurrentTexture == nullptr)
	{
		FPlatformProcess::Sleep(0.02);
		if (Stopping)
			return false;
		Timeout -= 0.02;
		if (Timeout < 0)
			return false;
	}

	CurrentTexture->PlatformData = new FTexturePlatformData();
	CurrentTexture->PlatformData->SizeX = InSizeX;
	CurrentTexture->PlatformData->SizeY = InSizeY;
	CurrentTexture->PlatformData->PixelFormat = InFormat;

	int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
	int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY;
	FTexture2DMipMap* Mip = new(CurrentTexture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = InSizeX;
	Mip->SizeY = InSizeY;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
	FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
	Mip->BulkData.Unlock();

	CurrentTexture->UpdateResource();
	return true;
}