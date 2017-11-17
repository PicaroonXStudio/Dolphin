// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "DolphinEditor.h"
#include "DolphinEditorStyle.h"
#include "DolphinEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "DolphinProtocolSelectorCustomization.h"

#include "LevelEditor.h"

static const FName DolphinEditorTabName("DolphinEditor");

#define LOCTEXT_NAMESPACE "FDolphinEditorModule"

void FDolphinEditorModule::StartupModule()
{
	PropertyModule.RegisterCustomPropertyTypeLayout(
		"DolphinResponseCallbackBinder",
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(
			&FDolphinResponseCallbackBinderCustomization::MakeInstance)
	);

}

void FDolphinEditorModule::ShutdownModule()
{
	PropertyModule.UnregisterCustomPropertyTypeLayout(
		"DolphinResponseCallbackBinder"
	);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDolphinEditorModule, DolphinEditor)