using UnrealBuildTool;
using System.IO;
 
public class AdvancedSessions : ModuleRules
{
    public AdvancedSessions(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "AdvancedSessions/Private"/*, "OnlineSubsystemSteam/Private"*/ });
        PublicIncludePaths.AddRange(new string[] { "AdvancedSessions/Public" });
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem","CoreUObject", "OnlineSubsystemUtils", "Networking", "Sockets", "Steamworks"/*, "OnlineSubsystemSteam"*/ });
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "Sockets", "Networking"/*, "Steamworks", "OnlineSubsystemSteam"*/ });
    }
}