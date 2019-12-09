// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
#include "steam/isteamugc.h"
#include "steam/isteamremotestorage.h"
#endif
#include "Interfaces/OnlineSessionInterface.h"

// @todo Steam: Steam headers trigger secure-C-runtime warnings in Visual C++. Rather than mess with _CRT_SECURE_NO_WARNINGS, we'll just
//	disable the warnings locally. Remove when this is fixed in the SDK
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
// #TODO check back on this at some point
#pragma warning(disable:4265) // SteamAPI CCallback< specifically, this warning is off by default but 4.17 turned it on....
#endif

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

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


#include "AdvancedSteamWorkshopLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSteamWorkshopLog, Log, All);


// Using a custom struct because uint32 isn't blueprint supported and I don't want to cast to int32
// due to the size of the workshop it could end up overflowing?
USTRUCT(BlueprintType)
struct FBPSteamWorkshopID
{
	GENERATED_USTRUCT_BODY()

public:

	uint64 SteamWorkshopID;

	FBPSteamWorkshopID()
	{

	}

	FBPSteamWorkshopID(uint64 ID)
	{
		SteamWorkshopID = ID;
	}
};


// General result codes - Copying steams version over
// Check these to future proof
UENUM(BlueprintType)
enum class FBPSteamResult : uint8
{
	K_EResultInvalid = 0,
	k_EResultOK = 1,							// success
	k_EResultFail = 2,							// generic failure 
	k_EResultNoConnection = 3,					// no/failed network connection
	//	k_EResultNoConnectionRetry = 4,				// OBSOLETE - removed
	k_EResultInvalidPassword = 5,				// password/ticket is invalid
	k_EResultLoggedInElsewhere = 6,				// same user logged in elsewhere
	k_EResultInvalidProtocolVer = 7,			// protocol version is incorrect
	k_EResultInvalidParam = 8,					// a parameter is incorrect
	k_EResultFileNotFound = 9,					// file was not found
	k_EResultBusy = 10,							// called method busy - action not taken
	k_EResultInvalidState = 11,					// called object was in an invalid state
	k_EResultInvalidName = 12,					// name is invalid
	k_EResultInvalidEmail = 13,					// email is invalid
	k_EResultDuplicateName = 14,				// name is not unique
	k_EResultAccessDenied = 15,					// access is denied
	k_EResultTimeout = 16,						// operation timed out
	k_EResultBanned = 17,						// VAC2 banned
	k_EResultAccountNotFound = 18,				// account not found
	k_EResultInvalidSteamID = 19,				// steamID is invalid
	k_EResultServiceUnavailable = 20,			// The requested service is currently unavailable
	k_EResultNotLoggedOn = 21,					// The user is not logged on
	k_EResultPending = 22,						// Request is pending (may be in process, or waiting on third party)
	k_EResultEncryptionFailure = 23,			// Encryption or Decryption failed
	k_EResultInsufficientPrivilege = 24,		// Insufficient privilege
	k_EResultLimitExceeded = 25,				// Too much of a good thing
	k_EResultRevoked = 26,						// Access has been revoked (used for revoked guest passes)
	k_EResultExpired = 27,						// License/Guest pass the user is trying to access is expired
	k_EResultAlreadyRedeemed = 28,				// Guest pass has already been redeemed by account, cannot be acked again
	k_EResultDuplicateRequest = 29,				// The request is a duplicate and the action has already occurred in the past, ignored this time
	k_EResultAlreadyOwned = 30,					// All the games in this guest pass redemption request are already owned by the user
	k_EResultIPNotFound = 31,					// IP address not found
	k_EResultPersistFailed = 32,				// failed to write change to the data store
	k_EResultLockingFailed = 33,				// failed to acquire access lock for this operation
	k_EResultLogonSessionReplaced = 34,
	k_EResultConnectFailed = 35,
	k_EResultHandshakeFailed = 36,
	k_EResultIOFailure = 37,
	k_EResultRemoteDisconnect = 38,
	k_EResultShoppingCartNotFound = 39,			// failed to find the shopping cart requested
	k_EResultBlocked = 40,						// a user didn't allow it
	k_EResultIgnored = 41,						// target is ignoring sender
	k_EResultNoMatch = 42,						// nothing matching the request found
	k_EResultAccountDisabled = 43,
	k_EResultServiceReadOnly = 44,				// this service is not accepting content changes right now
	k_EResultAccountNotFeatured = 45,			// account doesn't have value, so this feature isn't available
	k_EResultAdministratorOK = 46,				// allowed to take this action, but only because requester is admin
	k_EResultContentVersion = 47,				// A Version mismatch in content transmitted within the Steam protocol.
	k_EResultTryAnotherCM = 48,					// The current CM can't service the user making a request, user should try another.
	k_EResultPasswordRequiredToKickSession = 49,// You are already logged in elsewhere, this cached credential login has failed.
	k_EResultAlreadyLoggedInElsewhere = 50,		// You are already logged in elsewhere, you must wait
	k_EResultSuspended = 51,					// Long running operation (content download) suspended/paused
	k_EResultCancelled = 52,					// Operation canceled (typically by user: content download)
	k_EResultDataCorruption = 53,				// Operation canceled because data is ill formed or unrecoverable
	k_EResultDiskFull = 54,						// Operation canceled - not enough disk space.
	k_EResultRemoteCallFailed = 55,				// an remote call or IPC call failed
	k_EResultPasswordUnset = 56,				// Password could not be verified as it's unset server side
	k_EResultExternalAccountUnlinked = 57,		// External account (PSN, Facebook...) is not linked to a Steam account
	k_EResultPSNTicketInvalid = 58,				// PSN ticket was invalid
	k_EResultExternalAccountAlreadyLinked = 59,	// External account (PSN, Facebook...) is already linked to some other account, must explicitly request to replace/delete the link first
	k_EResultRemoteFileConflict = 60,			// The sync cannot resume due to a conflict between the local and remote files
	k_EResultIllegalPassword = 61,				// The requested new password is not legal
	k_EResultSameAsPreviousValue = 62,			// new value is the same as the old one ( secret question and answer )
	k_EResultAccountLogonDenied = 63,			// account login denied due to 2nd factor authentication failure
	k_EResultCannotUseOldPassword = 64,			// The requested new password is not legal
	k_EResultInvalidLoginAuthCode = 65,			// account login denied due to auth code invalid
	k_EResultAccountLogonDeniedNoMail = 66,		// account login denied due to 2nd factor auth failure - and no mail has been sent
	k_EResultHardwareNotCapableOfIPT = 67,		// 
	k_EResultIPTInitError = 68,					// 
	k_EResultParentalControlRestricted = 69,	// operation failed due to parental control restrictions for current user
	k_EResultFacebookQueryError = 70,			// Facebook query returned an error
	k_EResultExpiredLoginAuthCode = 71,			// account login denied due to auth code expired
	k_EResultIPLoginRestrictionFailed = 72,
	k_EResultAccountLockedDown = 73,
	k_EResultAccountLogonDeniedVerifiedEmailRequired = 74,
	k_EResultNoMatchingURL = 75,
	k_EResultBadResponse = 76,					// parse failure, missing field, etc.
	k_EResultRequirePasswordReEntry = 77,		// The user cannot complete the action until they re-enter their password
	k_EResultValueOutOfRange = 78,				// the value entered is outside the acceptable range
	k_EResultUnexpectedError = 79,				// something happened that we didn't expect to ever happen
	k_EResultDisabled = 80,						// The requested service has been configured to be unavailable
	k_EResultInvalidCEGSubmission = 81,			// The set of files submitted to the CEG server are not valid !
	k_EResultRestrictedDevice = 82,				// The device being used is not allowed to perform this action
	k_EResultRegionLocked = 83,					// The action could not be complete because it is region restricted
	k_EResultRateLimitExceeded = 84,			// Temporary rate limit exceeded, try again later, different from k_EResultLimitExceeded which may be permanent
	k_EResultAccountLoginDeniedNeedTwoFactor = 85,	// Need two-factor code to login
	k_EResultItemDeleted = 86,					// The thing we're trying to access has been deleted
	k_EResultAccountLoginDeniedThrottle = 87,	// login attempt failed, try to throttle response to possible attacker
	k_EResultTwoFactorCodeMismatch = 88,		// two factor code mismatch
	k_EResultTwoFactorActivationCodeMismatch = 89,	// activation code for two-factor didn't match
	k_EResultAccountAssociatedToMultiplePartners = 90,	// account has been associated with multiple partners
	k_EResultNotModified = 91, // data not modified
};

// Check these to future proof
UENUM(BlueprintType)
enum class FBPWorkshopFileType : uint8
{
	k_EWorkshopFileTypeCommunity = 0,
	k_EWorkshopFileTypeMicrotransaction = 1,
	k_EWorkshopFileTypeCollection = 2,
	k_EWorkshopFileTypeArt = 3,
	k_EWorkshopFileTypeVideo = 4,
	k_EWorkshopFileTypeScreenshot = 5,
	k_EWorkshopFileTypeGame = 6,
	k_EWorkshopFileTypeSoftware = 7,
	k_EWorkshopFileTypeConcept = 8,
	k_EWorkshopFileTypeWebGuide = 9,
	k_EWorkshopFileTypeIntegratedGuide = 10,
	k_EWorkshopFileTypeMerch = 11,
	k_EWorkshopFileTypeControllerBinding = 12,
	k_EWorkshopFileTypeSteamworksAccessInvite = 13,
	k_EWorkshopFileTypeSteamVideo = 14,

	// Update k_EWorkshopFileTypeMax if you add values.
	k_EWorkshopFileTypeMax = 15
};

// WorkshopItemDetails Struct
USTRUCT(BlueprintType)
struct FBPSteamWorkshopItemDetails
{
	GENERATED_USTRUCT_BODY()

public:

	FBPSteamWorkshopItemDetails()
	{
		ResultOfRequest = FBPSteamResult::k_EResultOK;
		FileType = FBPWorkshopFileType::k_EWorkshopFileTypeMax;
		CreatorAppID = 0;
		ConsumerAppID = 0;
		VotesUp = 0;
		VotesDown = 0;
		CalculatedScore = 0.f;
		bBanned = false;
		bAcceptedForUse = false;
		bTagsTruncated = false;
	}

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	FBPSteamWorkshopItemDetails(SteamUGCDetails_t &hUGCDetails)
	{
		ResultOfRequest = (FBPSteamResult)hUGCDetails.m_eResult;
		FileType = (FBPWorkshopFileType)hUGCDetails.m_eFileType;
		CreatorAppID = (int32)hUGCDetails.m_nCreatorAppID;
		ConsumerAppID = (int32)hUGCDetails.m_nConsumerAppID;
		Title = FString(hUGCDetails.m_rgchTitle, k_cchPublishedDocumentTitleMax);
		Description = FString(hUGCDetails.m_rgchDescription, k_cchPublishedDocumentDescriptionMax);
		ItemUrl = FString(hUGCDetails.m_rgchURL, k_cchPublishedFileURLMax);
		VotesUp = (int32)hUGCDetails.m_unVotesUp;
		VotesDown = (int32)hUGCDetails.m_unVotesDown;
		CalculatedScore = hUGCDetails.m_flScore;
		bBanned = hUGCDetails.m_bBanned;
		bAcceptedForUse = hUGCDetails.m_bAcceptedForUse;
		bTagsTruncated = hUGCDetails.m_bTagsTruncated;

		CreatorSteamID = FString::Printf(TEXT("%llu"), hUGCDetails.m_ulSteamIDOwner);
	}

	FBPSteamWorkshopItemDetails(const SteamUGCDetails_t &hUGCDetails)
	{
		ResultOfRequest = (FBPSteamResult)hUGCDetails.m_eResult;
		FileType = (FBPWorkshopFileType)hUGCDetails.m_eFileType;
		CreatorAppID = (int32)hUGCDetails.m_nCreatorAppID;
		ConsumerAppID = (int32)hUGCDetails.m_nConsumerAppID;
		Title = FString(hUGCDetails.m_rgchTitle, k_cchPublishedDocumentTitleMax);
		Description = FString(hUGCDetails.m_rgchDescription, k_cchPublishedDocumentDescriptionMax);
		ItemUrl = FString(hUGCDetails.m_rgchURL, k_cchPublishedFileURLMax);
		VotesUp = (int32)hUGCDetails.m_unVotesUp;
		VotesDown = (int32)hUGCDetails.m_unVotesDown;
		CalculatedScore = hUGCDetails.m_flScore;
		bBanned = hUGCDetails.m_bBanned;
		bAcceptedForUse = hUGCDetails.m_bAcceptedForUse;
		bTagsTruncated = hUGCDetails.m_bTagsTruncated;

		CreatorSteamID = FString::Printf(TEXT("%llu"), hUGCDetails.m_ulSteamIDOwner);
	}
#endif

	// Result of obtaining the details
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FBPSteamResult ResultOfRequest;

	// Type of file
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FBPWorkshopFileType FileType;

	// These two are listed as baked to an int, but is stored as a uint, think its safe to keep int
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		int32 CreatorAppID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		int32 ConsumerAppID;

	// Title of item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString Title;

	// Description of item
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString Description;
	
	//Url for a video of website
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
		FString ItemUrl;

	// Votes will be unlikely to go above signed limited
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	int32 VotesUp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	int32 VotesDown;

	// Calculated score
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	float CalculatedScore;

	// whether the file was banned
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bBanned;													
	
	// developer has specifically flagged this item as accepted in the Workshop
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bAcceptedForUse;	
	
	// whether the list of tags was too long to be returned in the provided buffer
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	bool bTagsTruncated;

	// Steam ID of the user who created this content.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Online|AdvancedSteamWorkshop")
	FString CreatorSteamID;

	/*
	PublishedFileId_t m_nPublishedFileId;
	uint32 m_rtimeCreated;											// time when the published file was created
	uint32 m_rtimeUpdated;											// time when the published file was last updated
	uint32 m_rtimeAddedToUserList;									// time when the user added the published file to their list (not always applicable)
	ERemoteStoragePublishedFileVisibility m_eVisibility;			// visibility
	char m_rgchTags[k_cchTagListMax];								// comma separated list of all tags associated with this file
	// file/url information
	UGCHandle_t m_hFile;											// The handle of the primary file
	UGCHandle_t m_hPreviewFile;										// The handle of the preview file
	char m_pchFileName[k_cchFilenameMax];							// The cloud filename of the primary file
	int32 m_nFileSize;												// Size of the primary file
	int32 m_nPreviewFileSize;										// Size of the preview file
	uint32 m_unNumChildren;											// if m_eFileType == k_EWorkshopFileTypeCollection, then this number will be the number of children contained within the collection
	*/

};

UCLASS()
class UAdvancedSteamWorkshopLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Steam Functions *************//
	
	// Returns IDs for subscribed workshop items, TArray length dictates how many
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static TArray<FBPSteamWorkshopID> GetSubscribedWorkshopItems(int32 & NumberOfItems);

	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static void GetNumSubscribedWorkshopItems(int32 & NumberOfItems);

};	
