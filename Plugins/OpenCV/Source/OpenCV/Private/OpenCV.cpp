// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OpenCV.h"
#include "Paths.h"
#include "PlatformProcess.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FOpenCVModule"

void FOpenCVModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("OpenCV")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPathA = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/opencv_world342.dll"));
	FString LibraryPathB = FPaths::Combine(*BaseDir, TEXT("Binaries/Win64/opencv_ffmpeg342_64.dll"));

	DllHandleA = FPlatformProcess::GetDllHandle(*LibraryPathA);
	DllHandleB = FPlatformProcess::GetDllHandle(*LibraryPathB);
}

void FOpenCVModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (DllHandleA)
		FPlatformProcess::FreeDllHandle(DllHandleA);

	if (DllHandleB)
		FPlatformProcess::FreeDllHandle(DllHandleB);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOpenCVModule, OpenCV)
