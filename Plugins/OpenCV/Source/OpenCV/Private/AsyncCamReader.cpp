#include "AsyncCamReader.h"
#include "PlatformProcess.h"
#include "Engine/Texture2D.h"

#include "opencv2/imgproc.hpp"

#include "Engine/Engine.h"


FAsyncCamReader::FAsyncCamReader(FString ThreadName, FString InCameraIP, int32 InCameraID, float InRefreshTime, bool bResize, FVector2D NewSize)
{
	// Initialize OpenCV and webcam properties
	CameraIP = InCameraIP;
	CameraID = InCameraID;
	RefreshTime = InRefreshTime;
	VideoSize = FVector2D(0, 0);
	ShouldResize = bResize;
	newsize = cv::Size(NewSize.X, NewSize.Y);
	stream = cv::VideoCapture();
	frame = cv::Mat();
	HasTexture = false;

	Thread = FRunnableThread::Create(this, *ThreadName);
}

FAsyncCamReader::~FAsyncCamReader()
{
	stream.~VideoCapture();
	frame.~Mat();

	if (Thread != nullptr)
	{
		Thread->Kill(false);
		delete Thread;
	}
}

bool FAsyncCamReader::Init()
{
	return true;
}

uint32 FAsyncCamReader::Run()
{
	// Open the stream
	if (!CameraIP.IsEmpty())
	{
		stream.open(TCHAR_TO_UTF8(*CameraIP));
	}
	else
	{
		stream.open(CameraID);
	}

	if (stream.isOpened())
	{
		// Initialize stream
		UpdateFrame();

		VideoSize = FVector2D(frame.cols, frame.rows);
		VideoTexture = UTexture2D::CreateTransient(VideoSize.X, VideoSize.Y);
		VideoTexture->UpdateResource();
		VideoUpdateTextureRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, VideoSize.X, VideoSize.Y);

		// Initialize data array
		Data.Init(FColor(0, 0, 0, 255), VideoSize.X * VideoSize.Y);

		// Do first frame
		DoProcessing();
		UpdateTexture();
		HasTexture = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, FString("Failed to open camera"));
		Stop();
	}

	FPlatformProcess::Sleep(0.03);

	while (StopTaskCounter.GetValue() == 0)
	{
		if (stream.isOpened())
		{
			UpdateFrame();
			DoProcessing();
			UpdateTexture();
		}

		FPlatformProcess::Sleep(RefreshTime);
	}
	return 0;
}

void FAsyncCamReader::Stop()
{
	StopTaskCounter.Increment();
}

void FAsyncCamReader::Exit()
{
	this->~FAsyncCamReader();
}

void FAsyncCamReader::UpdateFrame()
{
	stream.read(frame);

	if (ShouldResize)
	{
		cv::resize(frame, frame, newsize);
	}
}

void FAsyncCamReader::DoProcessing()
{

}

void FAsyncCamReader::UpdateTexture()
{
	if (frame.data)
	{
		// Copy Mat data to Data array
		for (int y = 0; y < VideoSize.Y; y++)
		{
			for (int x = 0; x < VideoSize.X; x++)
			{
				int i = x + (y * VideoSize.X);
				Data[i].B = frame.data[i * 3 + 0];
				Data[i].G = frame.data[i * 3 + 1];
				Data[i].R = frame.data[i * 3 + 2];
			}
		}

		// Update texture 2D
		VideoTexture->UpdateTextureRegions(0, 1, VideoUpdateTextureRegion, 4 * VideoSize.X, 4, (uint8*)Data.GetData());
	}
}
