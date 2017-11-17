// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "DolphinEditorStyle.h"

class FDolphinEditorCommands : public TCommands<FDolphinEditorCommands>
{
public:

	FDolphinEditorCommands()
		: TCommands<FDolphinEditorCommands>(TEXT("DolphinEditor"), NSLOCTEXT("Contexts", "DolphinEditor", "DolphinEditor Plugin"), NAME_None, FDolphinEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
