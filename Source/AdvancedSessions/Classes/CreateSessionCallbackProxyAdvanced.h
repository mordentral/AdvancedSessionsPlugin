// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "OnlineSubSystemHeader.h"

#include "CreateSessionCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UCreateSessionCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the session was created successfully
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnSuccess;

	// Called when there was an error creating the session
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// Creates a session with the default online subsystem with advanced optional inputs
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
		static UCreateSessionCallbackProxyAdvanced* CreateAdvancedSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> &ExtraSettings, class APlayerController* PlayerController = NULL, int32 PublicConnections = 100, bool bUseLAN = false, bool bAllowInvites = true, bool bIsDedicatedServer = false);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when session creation completes, calls StartSession
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);

	// Internal callback when session creation completes, calls StartSession
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);

	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The delegate executed by the online subsystem
	FOnCreateSessionCompleteDelegate CreateCompleteDelegate;

	// The delegate executed by the online subsystem
	FOnStartSessionCompleteDelegate StartCompleteDelegate;

	// Handles to the registered delegates above
	FDelegateHandle CreateCompleteDelegateHandle;
	FDelegateHandle StartCompleteDelegateHandle;

	// Number of public connections
	int NumPublicConnections;

	// Whether or not to search LAN
	bool bUseLAN;

	// Whether or not to allow invites
	bool bAllowInvites;

	// Whether this is a dedicated server or not
	bool bDedicatedServer;

	// Store extra settings
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};

