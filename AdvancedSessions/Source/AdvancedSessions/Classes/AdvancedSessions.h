#pragma once
 
#include "Modules/ModuleManager.h"

class AdvancedSessions : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};