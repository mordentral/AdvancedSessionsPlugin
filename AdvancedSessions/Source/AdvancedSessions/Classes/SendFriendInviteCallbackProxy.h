// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "SendFriendInviteCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedSendFriendInviteLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlueprintSendFriendInviteDelegate);

UCLASS(MinimalAPI)
class USendFriendInviteCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the friends list successfully was retrieved
	UPROPERTY(BlueprintAssignable)
	FBlueprintSendFriendInviteDelegate OnSuccess;

	// Called when there was an error retrieving the friends list
	UPROPERTY(BlueprintAssignable)
	FBlueprintSendFriendInviteDelegate OnFailure;

	// Adds a friend who is using the defined UniqueNetId, some interfaces do now allow this function to be called (INCLUDING STEAM)
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static USendFriendInviteCallbackProxy* SendFriendInvite(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &UniqueNetIDInvited);

	virtual void Activate() override;

private:
	// Internal callback when the friends list is retrieved
	void OnSendInviteComplete(int32 LocalPlayerNum, bool bWasSuccessful, const FUniqueNetId &InvitedPlayer, const FString &ListName, const FString &ErrorString);


	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The UniqueNetID of the person to invite
	FBPUniqueNetId cUniqueNetId;

	// The delegate to call on completion
	FOnSendInviteComplete OnSendInviteCompleteDelegate;

	// The world context object in which this call is taking place
	TWeakObjectPtr<UObject> WorldContextObject;
};

