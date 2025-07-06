// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "GetFriendsCallbackProxy.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Online.h"

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

	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("GetFriends"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		IOnlineFriendsPtr Friends = Helper.OnlineSub->GetFriendsInterface();
		if (Friends.IsValid())
		{
			ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

			Friends->ReadFriendsList(Player->GetControllerId(), EFriendsLists::ToString((EFriendsLists::Default)), FriendListReadCompleteDelegate);
			return;
		}
	}

	// Fail immediately
	TArray<FBPFriendInfo> EmptyArray;
	OnFailure.Broadcast(EmptyArray);
}

void UGetFriendsCallbackProxy::OnReadFriendsListCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString)
{
	if (bWasSuccessful)
	{
		FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("GetFriends"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
		Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

		if (!Helper.IsValid())
		{
			TArray<FBPFriendInfo> EmptyArray;
			OnFailure.Broadcast(EmptyArray);
			return;
		}

		auto Friends = Helper.OnlineSub->GetFriendsInterface();
		if (Friends.IsValid())
		{
			// Not actually needed anymore, plus was not being validated and causing a crash
			// ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);

			TArray<FBPFriendInfo> FriendsListOut;
			TArray<TSharedRef<FOnlineFriend>> FriendList;
			if (Friends->GetFriendsList(LocalUserNum, ListName, FriendList))
			{
				for (int32 i = 0; i < FriendList.Num(); i++)
				{
					FBPFriendInfo BPF;
					const FOnlineUserPresence& pres = FriendList[i]->GetPresence();
					BPF.OnlineState = ((EBPOnlinePresenceState)((int32)pres.Status.State));
					BPF.DisplayName = FriendList[i]->GetDisplayName();
					BPF.RealName = FriendList[i]->GetRealName();
					BPF.UniqueNetId.SetUniqueNetId(FriendList[i]->GetUserId());
					BPF.bIsPlayingSameGame = pres.bIsPlayingThisGame;

					BPF.PresenceInfo.bIsOnline = pres.bIsOnline;
					BPF.PresenceInfo.bHasVoiceSupport = pres.bHasVoiceSupport;
					BPF.PresenceInfo.bIsPlaying = pres.bIsPlaying;
					BPF.PresenceInfo.PresenceState = ((EBPOnlinePresenceState)((int32)pres.Status.State));

					// #TODO: Check back in on this in shipping, epic is missing the UTF8_TO_TCHAR call on converting this and its making an invalid string
					// BPF.PresenceInfo.StatusString = pres.Status.StatusStr;
					BPF.PresenceInfo.bIsJoinable = pres.bIsJoinable;
					BPF.PresenceInfo.bIsPlayingThisGame = pres.bIsPlayingThisGame;

					FriendsListOut.Add(BPF);
				}
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
