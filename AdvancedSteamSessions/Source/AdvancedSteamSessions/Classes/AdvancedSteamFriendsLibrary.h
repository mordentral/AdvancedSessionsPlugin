// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineFriendsInterface.h"
#include "OnlineUserInterface.h"
#include "OnlineMessageInterface.h"
#include "OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionInterface.h"

#include "UObjectIterator.h"

#include "AdvancedSteamFriendsLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSteamFriendsLog, Log, All);

UENUM(Blueprintable)
enum class SteamAvatarSize : uint8
{
	SteamAvatar_Small = 1,
	SteamAvatar_Medium = 2,
	SteamAvatar_Large = 3
};


UCLASS()
class UAdvancedSteamFriendsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Friend List Functions *************//

	// Get a texture of a valid friends avatar, STEAM ONLY, Returns invalid texture if the subsystem hasn't loaded that size of avatar yet
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI", meta = (ExpandEnumAsExecs = "Result"))
	static UTexture2D * GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize = SteamAvatarSize::SteamAvatar_Medium);

	// Preloads the avatar and name of a steam friend, return whether it is already available or not, STEAM ONLY, Takes time to actually load everything after this is called.
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static bool RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId, bool bRequireNameOnly = false);
	
	// Gets the level of a friends steam account, STEAM ONLY, Returns -1 if the steam level is not known, might need RequestSteamFriendInfo called first.
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static int32 GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId);

	// Gets the persona name of a steam ID, STEAM ONLY, Returns empty if no result, might need RequestSteamFriendInfo called first.
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static FString GetSteamPersonaName(const FBPUniqueNetId UniqueNetId);

	// Creates a unique steam id directly from a string holding a uint64 value, useful for testing
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|SteamAPI")
	static FBPUniqueNetId CreateSteamIDFromString(const FString SteamID64);
};	
