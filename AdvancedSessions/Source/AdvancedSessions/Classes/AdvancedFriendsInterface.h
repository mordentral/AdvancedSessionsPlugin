// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UObject/UObjectIterator.h"
#include "BlueprintDataDefinitions.h"
#include "AdvancedFriendsInterface.generated.h"


UINTERFACE(MinimalAPI)
class UAdvancedFriendsInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IAdvancedFriendsInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	
	// Called when the designated LocalUser has accepted a session invite, use JoinSession on result to connect
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSessionInviteReceived"))
	void OnSessionInviteReceived(FBPUniqueNetId PersonInviting, const FBlueprintSessionResult& SearchResult);

	// Called when the designated LocalUser has accepted a session invite, use JoinSession on result to connect
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSessionInviteAccepted"))
	void OnSessionInviteAccepted(FBPUniqueNetId PersonInvited, const FBlueprintSessionResult& SearchResult);

	// Called when the designated LocalUser has accepted a session invite, use JoinSession on result to connect
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerVoiceStateChanged"))
	void OnPlayerVoiceStateChanged(FBPUniqueNetId PlayerId, bool bIsTalking);

	// Called when the designated LocalUser has changed login state
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerLoginChanged"))
	void OnPlayerLoginChanged(int32 PlayerNum);

	// Called when the designated LocalUser has changed login state
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerLoginStatusChanged"))
	void OnPlayerLoginStatusChanged(EBPLoginStatus PreviousStatus, EBPLoginStatus NewStatus, FBPUniqueNetId PlayerUniqueNetID);

	// REMOVED BECAUSE IT WAS NEVER BEING CALLED
	// Called when the designated LocalUser has received a session invite, use JoinSession on result to connect
	//UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "OnSessionInviteReceived"))
	//void OnSessionInviteReceived(const FBPUniqueNetId &FriendInviting, const FBlueprintSessionResult &Session);

};	
