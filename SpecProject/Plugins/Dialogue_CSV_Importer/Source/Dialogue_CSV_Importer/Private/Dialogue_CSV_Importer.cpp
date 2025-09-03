// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dialogue_CSV_Importer.h"

#define LOCTEXT_NAMESPACE "FDialogue_CSV_ImporterModule"

void FDialogue_CSV_ImporterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDialogue_CSV_ImporterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDialogue_CSV_ImporterModule, Dialogue_CSV_Importer)