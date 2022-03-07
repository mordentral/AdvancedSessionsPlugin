#pragma once
 
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class AdvancedSteamSessions : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	void StartupModule();
	void ShutdownModule();
};