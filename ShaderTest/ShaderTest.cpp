// Fill out your copyright notice in the Description page of Project Settings.

#include "ShaderTest.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"
#include "Paths.h"


void FShaderTestModule::StartupModule()
{
	AddShaderSourceDirectoryMapping(FString("/Project"), FPaths::ProjectDir() + "Shaders");
}

IMPLEMENT_MODULE( FShaderTestModule, ShaderTest );
