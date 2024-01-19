// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "UpdateSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UUpdateSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the session was updated successfully
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error updating the session
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// Creates a session with the default online subsystem with advanced optional inputs, you MUST fill in all categories or it will pass in values that you didn't want as default values
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
	static UUpdateSessionCallbackProxyAdvanced* UpdateSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> &ExtraSettings, int32 PublicConnections = 100, int32 PrivateConnections = 0, bool bUseLAN = false, bool bAllowInvites = false, bool bAllowJoinInProgress = false, bool bRefreshOnlineData = true, bool bIsDedicatedServer = false, bool bShouldAdvertise = true);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when session creation completes, calls StartSession
	void OnUpdateCompleted(FName SessionName, bool bWasSuccessful);

	// The delegate executed by the online subsystem
	FOnUpdateSessionCompleteDelegate OnUpdateSessionCompleteDelegate;

	// Handles to the registered delegates above
	FDelegateHandle OnUpdateSessionCompleteDelegateHandle;

	// Number of public connections
	int NumPublicConnections = 100;

	// Number of private connections
	int NumPrivateConnections = 0;

	// Whether or not to search LAN
	bool bUseLAN = false;

	// Whether or not to allow invites
	bool bAllowInvites = true;

	// Store extra settings
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// Whether to update the online data
	bool bRefreshOnlineData = true;

	// Allow joining in progress
	bool bAllowJoinInProgress = true;

	// Update whether this is a dedicated server or not
	bool bDedicatedServer = false;

	bool bShouldAdvertise = true;

	// The world context object in which this call is taking place
	TWeakObjectPtr<UObject> WorldContextObject;
};

