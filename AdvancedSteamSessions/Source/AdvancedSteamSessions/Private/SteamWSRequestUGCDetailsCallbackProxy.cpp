// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubSystemHeader.h"
#include "SteamWSRequestUGCDetailsCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UEndSessionCallbackProxy

USteamWSRequestUGCDetailsCallbackProxy::USteamWSRequestUGCDetailsCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NumSecondsBeforeTimeout = 2.0f;
}


USteamWSRequestUGCDetailsCallbackProxy* USteamWSRequestUGCDetailsCallbackProxy::GetWorkshopItemDetails(UObject* WorldContextObject, FBPSteamWorkshopID WorkShopID, int32 NumSecondsBeforeTimeout)
{
	USteamWSRequestUGCDetailsCallbackProxy* Proxy = NewObject<USteamWSRequestUGCDetailsCallbackProxy>();

	Proxy->WorkShopID = WorkShopID;
	Proxy->NumSecondsBeforeTimeout = NumSecondsBeforeTimeout;
	return Proxy;
}

void USteamWSRequestUGCDetailsCallbackProxy::Activate()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		SteamAPICall_t hSteamAPICall = SteamUGC()->RequestUGCDetails(WorkShopID.SteamWorkshopID, NumSecondsBeforeTimeout);
		m_callResultUGCRequestDetails.Set(hSteamAPICall, this, &USteamWSRequestUGCDetailsCallbackProxy::OnUGCRequestUGCDetails);
		return;
	}
#endif
	OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
}

void USteamWSRequestUGCDetailsCallbackProxy::OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (bIOFailure || !pResult)
	{
		OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
		return;
	}

	OnSuccess.Broadcast(FBPSteamWorkshopItemDetails(pResult->m_details));
	return;
#endif

	OnFailure.Broadcast(FBPSteamWorkshopItemDetails());
}

