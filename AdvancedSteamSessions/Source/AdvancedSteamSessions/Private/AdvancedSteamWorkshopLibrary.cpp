// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedSteamWorkshopLibrary.h"
#include "OnlineSubSystemHeader.h"
//General Log
DEFINE_LOG_CATEGORY(AdvancedSteamWorkshopLog);


void UAdvancedSteamWorkshopLibrary::GetNumSubscribedWorkshopItems(int32 & NumberOfItems)
{
	NumberOfItems = 0;
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		NumberOfItems = SteamUGC()->GetNumSubscribedItems();
		return;
	}
	else
	{
		UE_LOG(AdvancedSteamWorkshopLog, Warning, TEXT("Error in GetNumSubscribedWorkshopItemCount : SteamAPI is not Inited!"));
		return;
	}
#endif

	UE_LOG(AdvancedSteamWorkshopLog, Warning, TEXT("Error in GetNumSubscribedWorkshopItemCount : Called on an incompatible platform"));
	return;
}

TArray<FBPSteamWorkshopID> UAdvancedSteamWorkshopLibrary::GetSubscribedWorkshopItems(int32 & NumberOfItems)
{
	TArray<FBPSteamWorkshopID> outArray;
	NumberOfItems = 0;

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		uint32 NumItems = SteamUGC()->GetNumSubscribedItems();
		
		if (NumItems == 0)
			return outArray;

		// Not using the actual variable above in case someone somehow goes past int32 limits
		// Don't want to go negative on the iteration.
		NumberOfItems = NumItems;

		PublishedFileId_t *fileIds = new PublishedFileId_t[NumItems];
		
		uint32 subItems = SteamUGC()->GetSubscribedItems(fileIds, NumItems);

		for (uint32 i = 0; i < subItems; ++i)
		{
			outArray.Add(FBPSteamWorkshopID(fileIds[i]));
		}

		delete[] fileIds;

		return outArray;
	}
	else
	{
		UE_LOG(AdvancedSteamWorkshopLog, Warning, TEXT("Error in GetSubscribedWorkshopItemCount : SteamAPI is not Inited!"));
		return outArray;
	}
#endif

	UE_LOG(AdvancedSteamWorkshopLog, Warning, TEXT("Error in GetSubscribedWorkshopItemCount : Called on an incompatible platform"));
	return outArray;
}
