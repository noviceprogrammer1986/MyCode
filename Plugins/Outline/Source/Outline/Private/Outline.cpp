// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Outline.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
extern UNREALED_API UEditorEngine* GEditor;
#include "TestButtonStyle.h"
#include "TestButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "EngineUtils.h"
#include "ProceduralMeshGenerator.h"

static const FName TestButtonTabName("OutlineHelper");
#endif

#define LOCTEXT_NAMESPACE "FOutlineModule"

void FOutlineModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	FTestButtonStyle::Initialize();
	FTestButtonStyle::ReloadTextures();

	FTestButtonCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTestButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FOutlineModule::PluginButtonClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FOutlineModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FOutlineModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
#endif
}

void FOutlineModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if WITH_EDITOR
	FTestButtonStyle::Shutdown();

	FTestButtonCommands::Unregister();
#endif
}

#if WITH_EDITOR
void FOutlineModule::PluginButtonClicked()
{
	TActorIterator<AProceduralMeshGenerator> Itr(GEditor->GetEditorWorldContext().World());
	if (Itr)
	{
		Itr->AutoFill();
	}
}

void FOutlineModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTestButtonCommands::Get().PluginAction);
}

void FOutlineModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTestButtonCommands::Get().PluginAction);
}
#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOutlineModule, Outline)