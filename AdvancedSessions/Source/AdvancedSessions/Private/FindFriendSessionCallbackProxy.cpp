// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "FindFriendSessionCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// UGetRecentPlayersCallbackProxy
DEFINE_LOG_CATEGORY(AdvancedFindFriendSessionLog);

UFindFriendSessionCallbackProxy::UFindFriendSessionCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnFindFriendSessionCompleteDelegate(FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnFindFriendSessionCompleted))
{
}

UFindFriendSessionCallbackProxy* UFindFriendSessionCallbackProxy::FindFriendSession(UObject* WorldContextObject, APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId)
{
	UFindFriendSessionCallbackProxy* Proxy = NewObject<UFindFriendSessionCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->cUniqueNetId = FriendUniqueNetId;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UFindFriendSessionCallbackProxy::Activate()
{
	if (!cUniqueNetId.IsValid())
	{
		// Fail immediately
		UE_LOG(AdvancedFindFriendSessionLog, Warning, TEXT("FindFriendSession Failed received a bad UniqueNetId!"));
		TArray<FBlueprintSessionResult> EmptyResult;
		OnFailure.Broadcast(EmptyResult);
		return;
	}

	if (!PlayerControllerWeakPtr.IsValid())
	{
		// Fail immediately
		UE_LOG(AdvancedFindFriendSessionLog, Warning, TEXT("FindFriendSession Failed received a bad playercontroller!"));
		TArray<FBlueprintSessionResult> EmptyResult;
		OnFailure.Broadcast(EmptyResult);
		return;
	}

	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());

	if (Sessions.IsValid())
	{	
		ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerControllerWeakPtr->Player);
		
		if (!Player)
		{
			// Fail immediately
			UE_LOG(AdvancedFindFriendSessionLog, Warning, TEXT("FindFriendSession Failed couldn't cast to ULocalPlayer!"));
			TArray<FBlueprintSessionResult> EmptyResult;
			OnFailure.Broadcast(EmptyResult);
			return;
		}

		FindFriendSessionCompleteDelegateHandle = Sessions->AddOnFindFriendSessionCompleteDelegate_Handle(Player->GetControllerId(), OnFindFriendSessionCompleteDelegate);

		Sessions->FindFriendSession(Player->GetControllerId(), *cUniqueNetId.GetUniqueNetId());

		return;
	}

	// Fail immediately
	TArray<FBlueprintSessionResult> EmptyResult;
	OnFailure.Broadcast(EmptyResult);
}


void UFindFriendSessionCallbackProxy::OnFindFriendSessionCompleted(int32 LocalPlayer, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SessionInfo)
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface(GetWorld());

	if (Sessions.IsValid())
		Sessions->ClearOnFindFriendSessionCompleteDelegate_Handle(LocalPlayer, FindFriendSessionCompleteDelegateHandle);

	if ( bWasSuccessful )
	{ 
		TArray<FBlueprintSessionResult> Result;

		for (auto& Sesh : SessionInfo)
		{
			if (Sesh.IsValid())
			{
				FBlueprintSessionResult BSesh;
				BSesh.OnlineResult = Sesh;
				Result.Add(BSesh);
			}
		}

		if(Result.Num() > 0)
			OnSuccess.Broadcast(Result);
		else
		{
			UE_LOG(AdvancedFindFriendSessionLog, Warning, TEXT("FindFriendSession Failed, returned an invalid session."));
			OnFailure.Broadcast(Result);
		}
	}
	else
	{
		UE_LOG(AdvancedFindFriendSessionLog, Warning, TEXT("FindFriendSession Failed"));
		TArray<FBlueprintSessionResult> EmptyResult;
		OnFailure.Broadcast(EmptyResult);
	}
}
