// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "AutoLoginUserCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// ULoginUserCallbackProxy

UAutoLoginUserCallbackProxy::UAutoLoginUserCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UAutoLoginUserCallbackProxy* UAutoLoginUserCallbackProxy::AutoLoginUser(UObject* WorldContextObject, int32 LocalUserNum)
{
	UAutoLoginUserCallbackProxy* Proxy = NewObject<UAutoLoginUserCallbackProxy>();
	Proxy->LocalUserNumber = LocalUserNum;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UAutoLoginUserCallbackProxy::Activate()
{
	auto Identity = Online::GetIdentityInterface();

	if (Identity.IsValid())
	{
		DelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(LocalUserNumber, Delegate);
		Identity->AutoLogin(LocalUserNumber);
		return;
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UAutoLoginUserCallbackProxy::OnCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorVal)
{
	auto Identity = Online::GetIdentityInterface();

	if (Identity.IsValid())
	{
		Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, DelegateHandle);
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
}
