// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#if WITH_EDITOR
#include "TestButtonCommands.h"

#define LOCTEXT_NAMESPACE "FOutlineModule"

void FTestButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Outline", "Execute Outline action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
#endif