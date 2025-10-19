#pragma once

#include "Modules/ModuleManager.h"

class FDialogue_CSV_Importer_EditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};