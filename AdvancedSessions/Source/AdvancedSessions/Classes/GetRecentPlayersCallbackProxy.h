// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "GetRecentPlayersCallbackProxy.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(AdvancedGetRecentPlayersLog, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintGetRecentPlayersDelegate, const TArray<FBPOnlineRecentPlayer>&, Results);

UCLASS(MinimalAPI)
class UGetRecentPlayersCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when the friends list successfully was retrieved
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetRecentPlayersDelegate OnSuccess;

	// Called when there was an error retrieving the friends list
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetRecentPlayersDelegate OnFailure;

	// Gets the list of recent players from the OnlineSubsystem and returns it, can be retrieved later with GetStoredRecentPlayersList, can fail if no recent players are found
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedFriends")
	static UGetRecentPlayersCallbackProxy* GetAndStoreRecentPlayersList(UObject* WorldContextObject, const FBPUniqueNetId &UniqueNetId);

	virtual void Activate() override;

private:
	// Internal callback when the friends list is retrieved
	void OnQueryRecentPlayersCompleted(const FUniqueNetId &UserID, const FString &Namespace, bool bWasSuccessful, const FString& ErrorString);
	// Handle to the registered OnFindSessionsComplete delegate
	FDelegateHandle DelegateHandle;

	// The player controller triggering things
	//TWeakObjectPtr<APlayerController> PlayerControllerWeakPtr;

	// The UniqueNetID of the person to get recent players for
	FBPUniqueNetId cUniqueNetId;

	// The delegate executed
	FOnQueryRecentPlayersCompleteDelegate QueryRecentPlayersCompleteDelegate;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};

