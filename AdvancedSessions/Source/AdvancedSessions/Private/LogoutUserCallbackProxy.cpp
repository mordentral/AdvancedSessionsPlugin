// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LogoutUserCallbackProxy.h"

#include "Online.h"

//////////////////////////////////////////////////////////////////////////
// ULogoutUserCallbackProxy

ULogoutUserCallbackProxy::ULogoutUserCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnLogoutCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

ULogoutUserCallbackProxy* ULogoutUserCallbackProxy::LogoutUser(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	ULogoutUserCallbackProxy* Proxy = NewObject<ULogoutUserCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void ULogoutUserCallbackProxy::Activate()
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

	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("LogoutUser"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));

	if (!Helper.OnlineSub)
	{
		OnFailure.Broadcast();
		return;
	}
	auto Identity = Helper.OnlineSub->GetIdentityInterface();

	if (Identity.IsValid())
	{
		DelegateHandle = Identity->AddOnLogoutCompleteDelegate_Handle(Player->GetControllerId(), Delegate);
		Identity->Logout(Player->GetControllerId());
		return;
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void ULogoutUserCallbackProxy::OnCompleted(int LocalUserNum, bool bWasSuccessful)
{

	if (PlayerControllerWeakPtr.IsValid())
	{
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

		if (Player)
		{
			FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("LogoutUser"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));

			if (!Helper.OnlineSub)
			{
				OnFailure.Broadcast();
				return;
			}
			auto Identity = Helper.OnlineSub->GetIdentityInterface();

			if (Identity.IsValid())
			{
				Identity->ClearOnLogoutCompleteDelegate_Handle(Player->GetControllerId(), DelegateHandle);
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
