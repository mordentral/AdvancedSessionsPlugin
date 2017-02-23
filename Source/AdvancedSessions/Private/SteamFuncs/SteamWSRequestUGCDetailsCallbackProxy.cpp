// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubSystemHeader.h"
#include "SteamFuncs/SteamWSRequestUGCDetailsCallbackProxy.h"

//////////////////////////////////////////////////////////////////////////
// UEndSessionCallbackProxy

USteamWSRequestUGCDetailsCallbackProxy::USteamWSRequestUGCDetailsCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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
	OnFailure.Broadcast();
}

void USteamWSRequestUGCDetailsCallbackProxy::OnUGCRequestUGCDetails(SteamUGCRequestUGCDetailsResult_t *pResult, bool bIOFailure)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (bIOFailure)
	{
		OnFailure.Broadcast();
		return;
	}

	SteamUGCDetails_t hUGCDetails = pResult->m_details;
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, hUGCDetails.m_rgchTitle);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, hUGCDetails.m_rgchDescription);

	OnSuccess.Broadcast();
	return;
#endif

	OnFailure.Broadcast();
	/*
		PublishedFileId_t m_nPublishedFileId;
	EResult m_eResult;												// The result of the operation.
	EWorkshopFileType m_eFileType;									// Type of the file
	AppId_t m_nCreatorAppID;										// ID of the app that created this file.
	AppId_t m_nConsumerAppID;										// ID of the app that will consume this file.
	char m_rgchTitle[k_cchPublishedDocumentTitleMax];				// title of document
	char m_rgchDescription[k_cchPublishedDocumentDescriptionMax];	// description of document
	uint64 m_ulSteamIDOwner;										// Steam ID of the user who created this content.
	uint32 m_rtimeCreated;											// time when the published file was created
	uint32 m_rtimeUpdated;											// time when the published file was last updated
	uint32 m_rtimeAddedToUserList;									// time when the user added the published file to their list (not always applicable)
	ERemoteStoragePublishedFileVisibility m_eVisibility;			// visibility
	bool m_bBanned;													// whether the file was banned
	bool m_bAcceptedForUse;											// developer has specifically flagged this item as accepted in the Workshop
	bool m_bTagsTruncated;											// whether the list of tags was too long to be returned in the provided buffer
	char m_rgchTags[k_cchTagListMax];								// comma separated list of all tags associated with this file
	// file/url information
	UGCHandle_t m_hFile;											// The handle of the primary file
	UGCHandle_t m_hPreviewFile;										// The handle of the preview file
	char m_pchFileName[k_cchFilenameMax];							// The cloud filename of the primary file
	int32 m_nFileSize;												// Size of the primary file
	int32 m_nPreviewFileSize;										// Size of the preview file
	char m_rgchURL[k_cchPublishedFileURLMax];						// URL (for a video or a website)
	// voting information
	uint32 m_unVotesUp;												// number of votes up
	uint32 m_unVotesDown;											// number of votes down
	float m_flScore;												// calculated score
	uint32 m_unNumChildren;											// if m_eFileType == k_EWorkshopFileTypeCollection, then this number will be the number of children contained within the collection
	
	
	*/
}

