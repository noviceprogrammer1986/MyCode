// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FToolBarBuilder;
class FMenuBuilder;

class FOutlineModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	/** This function will be bound to Command. */
	void PluginButtonClicked();

private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
#endif
};
