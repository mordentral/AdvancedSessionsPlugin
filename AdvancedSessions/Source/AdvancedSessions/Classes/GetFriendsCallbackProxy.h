// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Engine/LocalPlayer.h"
#include "GetFriendsCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedGetFriendsLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintGetFriendsListDelegate, const TArray<FBPFriendInfo>&, Results);

UCLASS(MinimalAPI)
class UGetFriendsCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the friends list successfully was retrieved
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetFriendsListDelegate OnSuccess;

	// Called when there was an error retrieving the friends list
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetFriendsListDelegate OnFailure;

	// Gets the players list of friends from the OnlineSubsystem and returns it, can be retrieved later with GetStoredFriendsList
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UGetFriendsCallbackProxy* GetAndStoreFriendsList(UObject* WorldContextObject, class APlayerController* PlayerController);

	virtual void Activate() override;

private:
	// Internal callback when the friends list is retrieved
	void OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString);

	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The delegate executed
	FOnReadFriendsListComplete FriendListReadCompleteDelegate;

	// The Type of friends list to get
	// Removed because all but the facebook interfaces don't even currently support anything but the default friends list.
	//EBPFriendsLists FriendListToGet;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};

