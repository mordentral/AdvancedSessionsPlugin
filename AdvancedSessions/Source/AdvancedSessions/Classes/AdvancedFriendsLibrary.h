// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedFriendsLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedFriendsLog, Log, All);

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
};	
