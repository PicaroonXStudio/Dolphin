// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DolphinEditorCommands.h"

#define LOCTEXT_NAMESPACE "FDolphinEditorModule"

void FDolphinEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "DolphinEditor", "Execute DolphinEditor action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
