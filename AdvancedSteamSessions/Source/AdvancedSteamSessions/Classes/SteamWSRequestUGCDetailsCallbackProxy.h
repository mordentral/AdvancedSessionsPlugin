// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AdvancedSteamWorkshopLibrary.h"
#include "BlueprintDataDefinitions.h"

// This is taken directly from UE4 - OnlineSubsystemSteamPrivatePCH.h as a fix for the array_count macro

// @todo Steam: Steam headers trigger secure-C-runtime warnings in Visual C++. Rather than mess with _CRT_SECURE_NO_WARNINGS, we'll just
//	disable the warnings locally. Remove when this is fixed in the SDK
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
// #TODO check back on this at some point
#pragma warning(disable:4265) // SteamAPI CCallback< specifically, this warning is off by default but 4.17 turned it on....
#endif

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

#include "OnlineSubsystemSteam.h"

#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

#if USING_CODE_ANALYSIS
MSVC_PRAGMA(warning(push))
MSVC_PRAGMA(warning(disable : ALL_CODE_ANALYSIS_WARNINGS))
#endif	// USING_CODE_ANALYSIS

#include <steam/steam_api.h>

#if USING_CODE_ANALYSIS
MSVC_PRAGMA(warning(pop))
#endif	// USING_CODE_ANALYSIS


#pragma pop_macro("ARRAY_COUNT")

#endif

// @todo Steam: See above
#ifdef _MSC_VER
#pragma warning(pop)
#endif


#include "SteamWSRequestUGCDetailsCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintWorkshopDetailsDelegate, const FBPSteamWorkshopItemDetails&, WorkShopDetails);

UCLASS(MinimalAPI)
class USteamWSRequestUGCDetailsCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()

	// Called when there is a successful results return
	UPROPERTY(BlueprintAssignable)
	FBlueprintWorkshopDetailsDelegate OnSuccess;

	// Called when there is an unsuccessful results return
	UPROPERTY(BlueprintAssignable)
	FBlueprintWorkshopDetailsDelegate OnFailure;

	// Ends the current session
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true", WorldContext="WorldContextObject"), Category = "Online|AdvancedSteamWorkshop")
	static USteamWSRequestUGCDetailsCallbackProxy* GetWorkshopItemDetails(UObject* WorldContextObject, FBPSteamWorkshopID WorkShopID);

	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface

private:
	
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	// Internal callback when the operation completes, calls out to the public success/failure callbacks
		
	void OnUGCRequestUGCDetails(SteamUGCQueryCompleted_t *pResult, bool bIOFailure);
	CCallResult<USteamWSRequestUGCDetailsCallbackProxy, SteamUGCQueryCompleted_t> m_callResultUGCRequestDetails;

#endif

private:

	FBPSteamWorkshopID WorkShopID;
	UObject* WorldContextObject;
};
