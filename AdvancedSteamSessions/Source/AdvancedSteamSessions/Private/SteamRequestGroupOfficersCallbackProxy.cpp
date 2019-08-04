// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SteamRequestGroupOfficersCallbackProxy.h"
#include "UObject/CoreOnline.h"
#include "AdvancedSteamFriendsLibrary.h"
#include "OnlineSubSystemHeader.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
#include "steam/isteamfriends.h"
#endif
//#include "OnlineSubsystemSteamTypes.h"

//////////////////////////////////////////////////////////////////////////
// UEndSessionCallbackProxy

USteamRequestGroupOfficersCallbackProxy::USteamRequestGroupOfficersCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

USteamRequestGroupOfficersCallbackProxy::~USteamRequestGroupOfficersCallbackProxy()
{
}

USteamRequestGroupOfficersCallbackProxy* USteamRequestGroupOfficersCallbackProxy::GetSteamGroupOfficerList(UObject* WorldContextObject, FBPUniqueNetId GroupUniqueNetID)
{
	USteamRequestGroupOfficersCallbackProxy* Proxy = NewObject<USteamRequestGroupOfficersCallbackProxy>();

	Proxy->GroupUniqueID = GroupUniqueNetID;
	return Proxy;
}

void USteamRequestGroupOfficersCallbackProxy::Activate()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)GroupUniqueID.UniqueNetId->GetBytes());
		SteamAPICall_t hSteamAPICall = SteamFriends()->RequestClanOfficerList(id);
	
		m_callResultGroupOfficerRequestDetails.Set(hSteamAPICall, this, &USteamRequestGroupOfficersCallbackProxy::OnRequestGroupOfficerDetails);
		return;
	}
#endif
	TArray<FBPSteamGroupOfficer> EmptyArray;
	OnFailure.Broadcast(EmptyArray);
}

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
void USteamRequestGroupOfficersCallbackProxy::OnRequestGroupOfficerDetails(ClanOfficerListResponse_t *pResult, bool bIOFailure)
{
	TArray<FBPSteamGroupOfficer> OfficerArray;
	
	FOnlineSubsystemSteam* SteamSubsystem = (FOnlineSubsystemSteam*)(IOnlineSubsystem::Get(STEAM_SUBSYSTEM));

	if (bIOFailure || !pResult || !pResult->m_bSuccess)
	{
		if (SteamSubsystem != nullptr)
		{
			SteamSubsystem->ExecuteNextTick([this]()
			{
				TArray<FBPSteamGroupOfficer> FailureArray;
				OnFailure.Broadcast(FailureArray);
			});
		}
		//OnFailure.Broadcast(OfficerArray);
		return;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)GroupUniqueID.UniqueNetId->GetBytes());

		FBPSteamGroupOfficer Officer;
		CSteamID ClanOwner = SteamFriends()->GetClanOwner(id);

		Officer.bIsOwner = true;

		TSharedPtr<const FUniqueNetId> ValueID(new const FUniqueNetIdSteam2(ClanOwner));
		Officer.OfficerUniqueNetID.SetUniqueNetId(ValueID);
		OfficerArray.Add(Officer);

		for (int i = 0; i < pResult->m_cOfficers; i++)
		{
			CSteamID OfficerSteamID = SteamFriends()->GetClanOfficerByIndex(id, i);

			Officer.bIsOwner = false;

			TSharedPtr<const FUniqueNetId> newValueID(new const FUniqueNetIdSteam2(OfficerSteamID));
			Officer.OfficerUniqueNetID.SetUniqueNetId(newValueID);

			OfficerArray.Add(Officer);
		}

		if (SteamSubsystem != nullptr)
		{
			SteamSubsystem->ExecuteNextTick([OfficerArray, this]()
			{
				OnSuccess.Broadcast(OfficerArray);
			});
		}

		//OnSuccess.Broadcast(OfficerArray);
		return;
	}
	else
	{
		if (SteamSubsystem != nullptr)
		{
			SteamSubsystem->ExecuteNextTick([this]()
			{
				TArray<FBPSteamGroupOfficer> FailureArray;
				OnFailure.Broadcast(FailureArray);
			});
		}
	}

	// Should never hit this anyway
	//OnFailure.Broadcast(OfficerArray);
}
#endif

