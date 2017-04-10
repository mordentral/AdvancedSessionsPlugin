// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "SendFriendInviteCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// UGetRecentPlayersCallbackProxy
DEFINE_LOG_CATEGORY(AdvancedSendFriendInviteLog);

USendFriendInviteCallbackProxy::USendFriendInviteCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnSendInviteCompleteDelegate(FOnSendInviteComplete::CreateUObject(this, &ThisClass::OnSendInviteComplete))
{
}

USendFriendInviteCallbackProxy* USendFriendInviteCallbackProxy::SendFriendInvite(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &UniqueNetIDInvited)
{
	USendFriendInviteCallbackProxy* Proxy = NewObject<USendFriendInviteCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->cUniqueNetId = UniqueNetIDInvited;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void USendFriendInviteCallbackProxy::Activate()
{
	if (!cUniqueNetId.IsValid())
	{
		// Fail immediately
		UE_LOG(AdvancedSendFriendInviteLog, Warning, TEXT("SendFriendInvite Failed received a bad UniqueNetId!"));
		OnFailure.Broadcast();
		return;
	}

	if (!PlayerControllerWeakPtr.IsValid())
	{
		// Fail immediately
		UE_LOG(AdvancedSendFriendInviteLog, Warning, TEXT("SendFriendInvite Failed received a bad playercontroller!"));
		OnFailure.Broadcast();
		return;
	}

	IOnlineFriendsPtr Friends = Online::GetFriendsInterface();
	if (Friends.IsValid())
	{	
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);
		
		if (!Player)
		{
			// Fail immediately
			UE_LOG(AdvancedSendFriendInviteLog, Warning, TEXT("SendFriendInvite Failed couldn't cast to ULocalPlayer!"));
			OnFailure.Broadcast();
			return;
		}

		Friends->SendInvite(Player->GetControllerId(), *cUniqueNetId.GetUniqueNetId(), EFriendsLists::ToString((EFriendsLists::Default)), OnSendInviteCompleteDelegate);
		return;
	}
	// Fail immediately
	OnFailure.Broadcast();
}

void USendFriendInviteCallbackProxy::OnSendInviteComplete(int32 LocalPlayerNum, bool bWasSuccessful, const FUniqueNetId &InvitedPlayer, const FString &ListName, const FString &ErrorString)
{
	if ( bWasSuccessful )
	{ 
		OnSuccess.Broadcast();
	}
	else
	{
		UE_LOG(AdvancedSendFriendInviteLog, Warning, TEXT("SendFriendInvite Failed with error: %s"), *ErrorString);
		OnFailure.Broadcast();
	}
}
