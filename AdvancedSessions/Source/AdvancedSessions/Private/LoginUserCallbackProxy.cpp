// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LoginUserCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// ULoginUserCallbackProxy

ULoginUserCallbackProxy::ULoginUserCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

ULoginUserCallbackProxy* ULoginUserCallbackProxy::LoginUser(UObject* WorldContextObject, class APlayerController* PlayerController, FString UserID, FString UserToken)
{
	ULoginUserCallbackProxy* Proxy = NewObject<ULoginUserCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->UserID = UserID;
	Proxy->UserToken = UserToken;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void ULoginUserCallbackProxy::Activate()
{

	if (!PlayerControllerWeakPtr.IsValid())
	{
		OnFailure.Broadcast();
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

	if (!Player)
	{
		OnFailure.Broadcast();
		return;
	}

	auto Identity = Online::GetIdentityInterface();

	if (Identity.IsValid())
	{
		DelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(Player->GetControllerId(), Delegate);
		FOnlineAccountCredentials AccountCreds(Identity->GetAuthType(), UserID, UserToken);
		Identity->Login(Player->GetControllerId(), AccountCreds);
		return;
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void ULoginUserCallbackProxy::OnCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& ErrorVal)
{
	if (PlayerControllerWeakPtr.IsValid())
	{
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

		if (Player)
		{
			auto Identity = Online::GetIdentityInterface();

			if (Identity.IsValid())
			{
				Identity->ClearOnLoginCompleteDelegate_Handle(Player->GetControllerId(), DelegateHandle);
			}
		}
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
