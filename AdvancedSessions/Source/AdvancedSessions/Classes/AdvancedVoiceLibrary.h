// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintDataDefinitions.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/VoiceInterface.h"
//#include "OnlineFriendsInterface.h"
//#include "OnlineUserInterface.h"
//#include "OnlineMessageInterface.h"
//#include "OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
//#include "OnlineSessionInterface.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedVoiceLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedVoiceLog, Log, All);


UCLASS()
class UAdvancedVoiceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Voice Library Functions *************//

	// Get if a headset is present for the specified local user
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo")
	static void IsHeadsetPresent(bool & bHasHeadset, uint8 LocalPlayerNum = 0);

	// Starts networked voice, allows push to talk in coordination with StopNetworkedVoice
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void StartNetworkedVoice(uint8 LocalPlayerNum = 0);

	// Stops networked voice, allows push to talk in coordination with StartNetworkedVoice
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void StopNetworkedVoice(uint8 LocalPlayerNum = 0);

	// Registers a local player as someone interested in voice data
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static bool RegisterLocalTalker(uint8 LocalPlayerNum = 0);

	// Registers all signed in players as local talkers
	// This is already done automatically, only do it manually if you unregistered someone
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void RegisterAllLocalTalkers();

	// UnRegisters local player as a local talker
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void UnRegisterLocalTalker(uint8 LocalPlayerNum = 0);

	// UnRegisters all signed in players as local talkers
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void UnRegisterAllLocalTalkers();

	// Registers a remote player as a talker
	// This is already done automatically, only do it manually if you unregistered someone
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static bool RegisterRemoteTalker(const FBPUniqueNetId& UniqueNetId);

	// UnRegisters a remote player as a talker
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static bool UnRegisterRemoteTalker(const FBPUniqueNetId& UniqueNetId);

	// UnRegisters all remote players as talkers
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static void RemoveAllRemoteTalkers();

	// Returns whether a local player is currently talking
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo")
	static bool IsLocalPlayerTalking(uint8 LocalPlayerNum);

	// Returns whether a remote player is currently talking
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo")
	static bool IsRemotePlayerTalking(const FBPUniqueNetId& UniqueNetId);

	// Returns whether a player is muted for the specified local player
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo")
	static bool IsPlayerMuted(uint8 LocalUserNumChecking, const FBPUniqueNetId& UniqueNetId);

	// Mutes the player associated with the uniquenetid for the specified local player, if IsSystemWide is true then it will attempt to mute globally for the player
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static bool MuteRemoteTalker(uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide = false);

	// UnMutes the player associated with the uniquenetid for the specified local player, if IsSystemWide is true then it will attempt to unmute globally for the player
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedVoice")
	static bool UnMuteRemoteTalker(uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide = false);

	// Gets the number of local talkers for this system
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedVoice|VoiceInfo")
	static void GetNumLocalTalkers(int32 & NumLocalTalkers);
};	
