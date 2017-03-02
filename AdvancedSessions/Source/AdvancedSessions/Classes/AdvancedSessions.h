#pragma once
 
#include "OnlineSubSystemHeader.h"
#include "ModuleManager.h"

class AdvancedSessions : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};