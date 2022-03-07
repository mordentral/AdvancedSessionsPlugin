// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "FindFriendSessionCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedFindFriendSessionLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintFindFriendSessionDelegate, const TArray<FBlueprintSessionResult> &, SessionInfo);

UCLASS(MinimalAPI)
class UFindFriendSessionCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the friends list successfully was retrieved
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindFriendSessionDelegate OnSuccess;

	// Called when there was an error retrieving the friends list
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindFriendSessionDelegate OnFailure;

	// Attempts to get the current session that a friend is in
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UFindFriendSessionCallbackProxy* FindFriendSession(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId);

	virtual void Activate() override;

private:
	// Internal callback when the friends list is retrieved
	void OnFindFriendSessionCompleted(int32 LocalPlayer, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SessionInfo);

	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The UniqueNetID of the person to invite
	FBPUniqueNetId cUniqueNetId;

	// The delegate to call on completion
	FOnFindFriendSessionCompleteDelegate OnFindFriendSessionCompleteDelegate;

	// Handles to the registered delegates above
	FDelegateHandle FindFriendSessionCompleteDelegateHandle;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};

