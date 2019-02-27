// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Outline.h"
#include "PropertyEditorModule.h"
#include "ProceduralMeshGenerator.h"
#include "ProceduralMeshGeneratorDetails.h"

#define LOCTEXT_NAMESPACE "FOutlineModule"

void FOutlineModule::StartupModule()
{
#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AProceduralMeshGenerator::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FProceduralMeshGeneratorDetails::MakeInstance));
#endif
}

void FOutlineModule::ShutdownModule()
{
#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(AProceduralMeshGenerator::StaticClass()->GetFName());
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOutlineModule, Outline)