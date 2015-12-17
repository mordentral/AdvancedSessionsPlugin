// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "OnlineSubSystemHeader.h"
#include "GetFriendsCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UGetFriendsCallbackProxy
DEFINE_LOG_CATEGORY(AdvancedGetFriendsLog);

UGetFriendsCallbackProxy::UGetFriendsCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, FriendListReadCompleteDelegate(FOnReadFriendsListComplete::CreateUObject(this, &ThisClass::OnReadFriendsListCompleted))
{
}

UGetFriendsCallbackProxy* UGetFriendsCallbackProxy::GetAndStoreFriendsList(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	UGetFriendsCallbackProxy* Proxy = NewObject<UGetFriendsCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UGetFriendsCallbackProxy::Activate()
{
	if (!PlayerControllerWeakPtr.IsValid())
	{
		// Fail immediately
		UE_LOG(AdvancedGetFriendsLog, Warning, TEXT("GetFriends Failed received a bad player controller!"));
		TArray<FBPFriendInfo> EmptyArray;
		OnFailure.Broadcast(EmptyArray);
		return;
	}

	IOnlineFriendsPtr Friends = Online::GetFriendsInterface();
	if (Friends.IsValid())
	{	
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

		Friends->ReadFriendsList(Player->GetControllerId(), EFriendsLists::ToString((EFriendsLists::Type::Default)), FriendListReadCompleteDelegate);
		return;
	}

	// Fail immediately
	TArray<FBPFriendInfo> EmptyArray;

	OnFailure.Broadcast(EmptyArray);
}

void UGetFriendsCallbackProxy::OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString)
{
	if (bWasSuccessful)
	{
		IOnlineFriendsPtr Friends = Online::GetFriendsInterface();
		if (Friends.IsValid())
		{
			ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

			TArray<FBPFriendInfo> FriendsListOut;
			TArray< TSharedRef<FOnlineFriend> > FriendList;
			Friends->GetFriendsList(LocalUserNum, ListName, FriendList);

			for (int32 i = 0; i < FriendList.Num(); i++)
			{
				TSharedRef<FOnlineFriend> Friend = FriendList[i];
				FBPFriendInfo BPF;
				BPF.OnlineState = ((EBPOnlinePresenceState::Type)((int32)Friend->GetPresence().Status.State));
				BPF.DisplayName = Friend->GetDisplayName();
				BPF.RealName = Friend->GetRealName();
				BPF.UniqueNetId.SetUniqueNetId(Friend->GetUserId());
				FriendsListOut.Add(BPF);
			}

			OnSuccess.Broadcast(FriendsListOut);
		}
	}
	else
	{
		TArray<FBPFriendInfo> EmptyArray;
		OnFailure.Broadcast(EmptyArray);
	}
}
