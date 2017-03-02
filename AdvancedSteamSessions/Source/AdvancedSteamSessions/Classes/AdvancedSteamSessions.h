#pragma once
 
#include "OnlineSubSystemHeader.h"
#include "ModuleManager.h"

class AdvancedSteamSessions : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};