// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "OnlineIdentityInterface.h"
#include "GetUserPrivilegeCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBlueprintGetUserPrivilegeDelegate,/* const &FBPUniqueNetId, PlayerID,*/ EBPUserPrivileges, QueriedPrivilege, bool, HadPrivilege);

UCLASS(MinimalAPI)
class UGetUserPrivilegeCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful destroy
	UPROPERTY(BlueprintAssignable)
	FBlueprintGetUserPrivilegeDelegate OnSuccess;

	// Called when there is an unsuccessful destroy
	UPROPERTY(BlueprintAssignable)
	FEmptyOnlineDelegate OnFailure;

	// Gets the privilage of the user
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedIdentity")
	static UGetUserPrivilegeCallbackProxy* GetUserPrivilege(UObject* WorldContextObject, const EBPUserPrivileges & PrivilegeToCheck, const FBPUniqueNetId & PlayerUniqueNetID);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	// Internal callback when the operation completes, calls out to the public success/failure callbacks
	void OnCompleted(const FUniqueNetId& PlayerID, EUserPrivileges::Type Privilege, uint32 Result);

private:
	// The player controller triggering things
	FBPUniqueNetId PlayerUniqueNetID;

	// Privilege to check
	EBPUserPrivileges UserPrivilege;

	// The world context object in which this call is taking place
	UObject* WorldContextObject;
};
