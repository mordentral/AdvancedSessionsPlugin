// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
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

	/**
	 *    Creates a session with the default online subsystem with advanced optional inputs, for dedicated servers leave UsePresence as false and set IsDedicatedServer to true. Dedicated servers don't use presence.
	 *    @param PublicConnections	When doing a 'listen' server, this must be >=2 (ListenServer itself counts as a connection)
	 *    @param bUseLAN			When you want to play LAN, the level to play on must be loaded with option 'bIsLanMatch'
	 *    @param bUsePresence		Must be true for a 'listen' server (Map must be loaded with option 'listen'), false for a 'dedicated' server.
	 *	  @param bUseLobbiesIfAvailable Used to flag the subsystem to use a lobby api instead of general hosting if the API supports it, generally true on steam for listen servers and false for dedicated
	 *	  @param bShouldAdvertise	Set to true when the OnlineSubsystem should list your server when someone is searching for servers. Otherwise the server is hidden and only join via invite is possible.
	 *	  @param bUseLobbiesVoiceChatIfAvailable Set to true to setup voice chat lobbies if the API supports it
	 *	  @param bStartAfterCreate Set to true to start the session after it's created.
	 */
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject",AutoCreateRefTerm="ExtraSettings"), Category = "Online|AdvancedSessions")
	static UCreateSessionCallbackProxyAdvanced* CreateAdvancedSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> &ExtraSettings, class APlayerController* PlayerController = NULL, int32 PublicConnections = 100, int32 PrivateConnections = 0, bool bUseLAN = false, bool bAllowInvites = true, bool bIsDedicatedServer = false, bool bUsePresence = true, bool bUseLobbiesIfAvailable = true, bool bAllowJoinViaPresence = true, bool bAllowJoinViaPresenceFriendsOnly = false, bool bAntiCheatProtected = false, bool bUsesStats = false, bool bShouldAdvertise = true, bool bUseLobbiesVoiceChatIfAvailable = false, bool bStartAfterCreate = true);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when session creation completes, calls StartSession
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);

	// Internal callback when session start completes
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

	// Number of private connections
	int NumPrivateConnections;

	// Whether or not to search LAN
	bool bUseLAN;

	// Whether or not to allow invites
	bool bAllowInvites;

	// Whether this is a dedicated server or not
	bool bDedicatedServer;

	// Whether to use the presence option
	bool bUsePresence;

	// Whether to prefer the use of lobbies for hosting if the api supports them
	bool bUseLobbiesIfAvailable;

	// Whether to allow joining via presence
	bool bAllowJoinViaPresence;

	// Allow joining via presence for friends only
	bool bAllowJoinViaPresenceFriendsOnly;

	// Delcare the server to be anti cheat protected
	bool bAntiCheatProtected;

	// Record Stats
	bool bUsesStats;

	// Should advertise server?
	bool bShouldAdvertise;

	// Whether to prefer the use of voice chat lobbies if the api supports them
	bool bUseLobbiesVoiceChatIfAvailable;

	// Whether to start the session automatically after it is created
	bool bStartAfterCreate;

	// Store extra settings
	TArray<FSessionPropertyKeyPair> ExtraSettings;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};

