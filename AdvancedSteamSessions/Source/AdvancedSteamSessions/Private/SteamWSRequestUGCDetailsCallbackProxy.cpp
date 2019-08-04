// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SteamWSRequestUGCDetailsCallbackProxy.h"
#include "OnlineSubSystemHeader.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
#include "steam/isteamugc.h"
#endif

//////////////////////////////////////////////////////////////////////////
// UEndSessionCallbackProxy

USteamWSRequestUGCDetailsCallbackProxy::USteamWSRequestUGCDetailsCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


USteamWSRequestUGCDetailsCallbackProxy* USteamWSRequestUGCDetailsCallbackProxy::GetWorkshopItemDetails(UObject* WorldContextObject, FBPSteamWorkshopID WorkShopID/*, int32 NumSecondsBeforeTimeout*/)
{
	USteamWSRequestUGCDetailsCallbackProxy* Proxy = NewObject<USteamWSRequestUGCDetailsCallbackProxy>();

	Proxy->WorkShopID = WorkShopID;
	return Proxy;
}

void USteamWSRequestUGCDetailsCallbackProxy::Activate()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		// #TODO: Support arrays instead in the future?
		UGCQueryHandle_t hQueryHandle = SteamUGC()->CreateQueryUGCDetailsRequest((PublishedFileId_t *)&WorkShopID.SteamWorkshopID, 1);
		// #TODO: add search settings here by calling into the handle?
		SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(hQueryHandle);

		// Need to release the query
		SteamUGC()->ReleaseQueryUGCRequest(hQueryHandle);

		if (hSteamAPICall == k_uAPICallInvalid)
		{
			OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
			return;
		}

		m_callResultUGCRequestDetails.Set(hSteamAPICall, this, &USteamWSRequestUGCDetailsCallbackProxy::OnUGCRequestUGCDetails);
		return;
	}
#endif
	OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
}

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
void USteamWSRequestUGCDetailsCallbackProxy::OnUGCRequestUGCDetails(SteamUGCQueryCompleted_t *pResult, bool bIOFailure)
{	
	FOnlineSubsystemSteam* SteamSubsystem = (FOnlineSubsystemSteam*)(IOnlineSubsystem::Get(STEAM_SUBSYSTEM));

	if (bIOFailure || !pResult || pResult->m_unNumResultsReturned <= 0)
	{
		if (SteamSubsystem != nullptr)
		{
			SteamSubsystem->ExecuteNextTick([this]()
			{
				OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
			});
		}
		//OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
		return;
	}
	if (SteamAPI_Init())
	{
		SteamUGCDetails_t Details;
		if (SteamUGC()->GetQueryUGCResult(pResult->m_handle, 0, &Details))
		{
			if (SteamSubsystem != nullptr)
			{
				SteamSubsystem->ExecuteNextTick([Details, this]()
				{
					OnSuccess.Broadcast(FBPSteamWorkshopItemDetails(Details));
				});
			}

			//OnSuccess.Broadcast(FBPSteamWorkshopItemDetails(Details));
			return;
		}
	}
	else
	{
		if (SteamSubsystem != nullptr)
		{
			SteamSubsystem->ExecuteNextTick([this]()
			{
				OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
			});
		}
	}

	// Not needed, should never hit here
	//OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
}
#endif

