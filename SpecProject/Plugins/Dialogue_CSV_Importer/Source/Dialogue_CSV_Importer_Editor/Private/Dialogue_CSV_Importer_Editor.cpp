#include "Dialogue_CSV_Importer_Editor.h"

#define LOCTEXT_NAMESPACE "FDialogue_CSV_Importer_EditorModule"

void FDialogue_CSV_Importer_EditorModule::StartupModule()
{
	// Codice editor-only eseguito all'avvio
}

void FDialogue_CSV_Importer_EditorModule::ShutdownModule()
{
	// Pulizia editor-only
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDialogue_CSV_Importer_EditorModule, Dialogue_CSV_Importer_Editor)