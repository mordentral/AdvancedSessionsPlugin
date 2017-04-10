#pragma once
 
#include "CoreMinimal.h"
#include "ModuleManager.h"

class AdvancedSteamSessions : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};