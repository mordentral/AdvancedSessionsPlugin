// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GetUserPrivilegeCallbackProxy.h"

#include "Online.h"

//////////////////////////////////////////////////////////////////////////
// UGetUserPrivilegeCallbackProxy

UGetUserPrivilegeCallbackProxy::UGetUserPrivilegeCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UGetUserPrivilegeCallbackProxy* UGetUserPrivilegeCallbackProxy::GetUserPrivilege(UObject* WorldContextObject, const EBPUserPrivileges & PrivilegeToCheck, const FBPUniqueNetId & PlayerUniqueNetID)
{
	UGetUserPrivilegeCallbackProxy* Proxy = NewObject<UGetUserPrivilegeCallbackProxy>();
	Proxy->PlayerUniqueNetID.SetUniqueNetId(PlayerUniqueNetID.GetUniqueNetId());
	Proxy->UserPrivilege = PrivilegeToCheck;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UGetUserPrivilegeCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("GetUserPrivilege"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));

	if (!Helper.OnlineSub)
	{
		OnFailure.Broadcast();
		return;
	}
	auto Identity = Helper.OnlineSub->GetIdentityInterface();

	if (Identity.IsValid())
	{
		Identity->GetUserPrivilege(*PlayerUniqueNetID.GetUniqueNetId(), (EUserPrivileges::Type)UserPrivilege, IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted));
		return;
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UGetUserPrivilegeCallbackProxy::OnCompleted(const FUniqueNetId& PlayerID, EUserPrivileges::Type Privilege, uint32 PrivilegeResult)
{
	OnSuccess.Broadcast(/*PlayerID,*/ (EBPUserPrivileges)Privilege, PrivilegeResult == 0);
}
