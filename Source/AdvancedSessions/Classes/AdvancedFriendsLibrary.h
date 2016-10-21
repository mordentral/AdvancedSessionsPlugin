// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSubSystemHeader.h"
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

#include "AdvancedFriendsLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedFriendsLog, Log, All);

UENUM(Blueprintable)
enum class SteamAvatarSize : uint8
{
	SteamAvatar_Small = 1,
	SteamAvatar_Medium = 2,
	SteamAvatar_Large = 3
};


UCLASS()
class UAdvancedFriendsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Friend List Functions *************//

	// Sends an Invite to the current online session to a list of friends
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList", meta = (ExpandEnumAsExecs = "Result"))
	static void SendSessionInviteToFriends(APlayerController *PlayerController, const TArray<FBPUniqueNetId> &Friends, EBlueprintResultSwitch &Result);

	// Sends an Invite to the current online session to a friend
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList", meta = (ExpandEnumAsExecs = "Result"))
	static void SendSessionInviteToFriend(APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId, EBlueprintResultSwitch &Result);

	// Get a friend from the previously read/saved friends list (Must Call GetFriends first for this to return anything)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList")
	static void GetFriend(APlayerController *PlayerController, const FBPUniqueNetId FriendUniqueNetId, FBPFriendInfo &Friend);

	// Get the previously read/saved friends list (Must Call GetFriends first for this to return anything)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|FriendsList")
	static void GetStoredFriendsList(APlayerController *PlayerController, TArray<FBPFriendInfo> &FriendsList);

	// Get the previously read/saved recent players list (Must Call GetRecentPlayers first for this to return anything)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|RecentPlayersList")
	static void GetStoredRecentPlayersList(FBPUniqueNetId UniqueNetId, TArray<FBPOnlineRecentPlayer> &PlayersList);

	// Check if a UniqueNetId is a friend
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedFriends|FriendsList")
	static void IsAFriend(APlayerController *PlayerController, const FBPUniqueNetId UniqueNetId, bool &IsFriend);

	// Get a texture of a valid friends avatar, STEAM ONLY, Returns invalid texture if the subsystem hasn't loaded that size of avatar yet
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static UTexture2D * GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, SteamAvatarSize AvatarSize = SteamAvatarSize::SteamAvatar_Medium);

	// Preloads the avatar and name of a steam friend, return whether it is already available or not, STEAM ONLY, Takes time to actually load everything after this is called.
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static bool RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId);
	
	// Gets the level of a friends steam account, STEAM ONLY, Returns -1 if the steam level is not known, might need RequestSteamFriendInfo called first.
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedFriends|SteamAPI")
	static int32 GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId);

};	
