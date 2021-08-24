// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "BlueprintDataDefinitions.h"
#include "FindSessionsCallbackProxyAdvanced.generated.h"

UCLASS(MinimalAPI)
class UFindSessionsCallbackProxyAdvanced : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful query
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnSuccess;

	// Called when there is an unsuccessful query
	UPROPERTY(BlueprintAssignable)
	FBlueprintFindSessionsResultDelegate OnFailure;

	// Searches for advertised sessions with the default online subsystem and includes an array of filters
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm="Filters"), Category = "Online|AdvancedSessions")
	static UFindSessionsCallbackProxyAdvanced* FindSessionsAdvanced(UObject* WorldContextObject, class APlayerController* PlayerController, int32 MaxResults, bool bUseLAN, EBPServerPresenceSearchType ServerTypeToSearch, const TArray<FSessionsSearchSetting> &Filters, bool bEmptyServersOnly = false, bool bNonEmptyServersOnly = false, bool bSecureServersOnly = false, bool bSearchLobbies = true, int MinSlotsAvailable = 0);

	static bool CompareVariants(const FVariantData &A, const FVariantData &B, EOnlineComparisonOpRedux Comparator);
	
	// Filters an array of session results by the given search parameters, returns a new array with the filtered results
	UFUNCTION(BluePrintCallable, meta = (Category = "Online|AdvancedSessions"))
	static void FilterSessionResults(const TArray<FBlueprintSessionResult> &SessionResults, const TArray<FSessionsSearchSetting> &Filters, TArray<FBlueprintSessionResult> &FilteredResults);
	
	// Removed, the default built in versions work fine in the normal FindSessionsCallbackProxy
	/*UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetPingInMs(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static FString GetServerName(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetCurrentPlayers(const FBlueprintSessionResult& Result);

	UFUNCTION(BlueprintPure, Category = "Online|Session")
	static int32 GetMaxPlayers(const FBlueprintSessionResult& Result);*/


	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when the session search completes, calls out to the public success/failure callbacks
	void OnCompleted(bool bSuccess);

	bool bRunSecondSearch;
	bool bIsOnSecondSearch;

	TArray<FBlueprintSessionResult> SessionSearchResults;

private:
	// The player controller triggering things
	TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The delegate executed by the online subsystem
	FOnFindSessionsCompleteDelegate Delegate;

	// Handle to the registered OnFindSessionsComplete delegate
	FDelegateHandle DelegateHandle;

	// Object to track search results
	TSharedPtr<FOnlineSessionSearch> SearchObject;
	TSharedPtr<FOnlineSessionSearch> SearchObjectDedicated;

	// Whether or not to search LAN
	bool bUseLAN;

	// Whether or not to search for dedicated servers
	EBPServerPresenceSearchType ServerSearchType;

	// Maximum number of results to return
	int MaxResults;

	// Store extra settings
	TArray<FSessionsSearchSetting> SearchSettings;

	// Search for empty servers only
	bool bEmptyServersOnly;

	// Search for non empty servers only
	bool bNonEmptyServersOnly;

	// Search for secure servers only
	bool bSecureServersOnly;

	// Search through lobbies
	bool bSearchLobbies;

	// Min slots requires to search
	int MinSlotsAvailable;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};
