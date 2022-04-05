using UnrealBuildTool;
using System.IO;
 
public class AdvancedSteamSessions : ModuleRules
{
    public AdvancedSteamSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //bEnforceIWYU = true;

        PublicDefinitions.Add("WITH_ADVANCED_STEAM_SESSIONS=1");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "CoreUObject", "OnlineSubsystemUtils", "Networking", "Sockets", "AdvancedSessions"/*"Voice", "OnlineSubsystemSteam"*/ });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking", "OnlineSubsystemUtils" /*"Voice", "Steamworks","OnlineSubsystemSteam"*/});

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Steamworks",/*"Voice",*/ "OnlineSubsystemSteam" });
            PublicIncludePaths.AddRange(new string[] { "../Plugins/Online/OnlineSubsystemSteam/Source/Private" });// This is dumb but it isn't very open
        }
    }
}