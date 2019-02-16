#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

#include "opencv2/core.hpp"
#include "opencv2/videoio.hpp"


class OPENCV_API FAsyncCamReader : public FRunnable
{

public:
	FAsyncCamReader(FString ThreadName, FString InCameraIP = "", int32 InCameraID = 0, float InRefreshTime = 0.03f,
					bool bResize = false, FVector2D NewSize = FVector2D(640, 480));
	~FAsyncCamReader();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	FRunnableThread* Thread;
	FThreadSafeCounter StopTaskCounter;

public:
	// The current video frame's corresponding texture
	class UTexture2D* VideoTexture;

	bool HasTexture;

	// The videos width and height (width, height)
	FVector2D VideoSize;

private:
	// The device ID opened by the Video Stream
	int32 CameraID;

	FString CameraIP;

	// The refresh time
	float RefreshTime;

	// OpenCV fields
	cv::Mat frame;
	cv::VideoCapture stream;

	// If the webcam images should be resized every frame 
	bool ShouldResize;

	// The targeted resize width and height (width, height) 
	cv::Size newsize;

	// The current data array
	TArray<FColor> Data;

	// Pointer to update texture region 2D struct
	struct FUpdateTextureRegion2D* VideoUpdateTextureRegion;

	// OpenCV prototypes
	void UpdateFrame();
	void DoProcessing();
	void UpdateTexture();
};
