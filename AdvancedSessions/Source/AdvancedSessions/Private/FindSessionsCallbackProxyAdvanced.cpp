// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "FindSessionsCallbackProxyAdvanced.h"

#include "Online/OnlineSessionNames.h"

//////////////////////////////////////////////////////////////////////////
// UFindSessionsCallbackProxyAdvanced


UFindSessionsCallbackProxyAdvanced::UFindSessionsCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
	, bUseLAN(false)
{
	bRunSecondSearch = false;
	bIsOnSecondSearch = false;
}

UFindSessionsCallbackProxyAdvanced* UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(UObject* WorldContextObject, class APlayerController* PlayerController, int MaxResults, bool bUseLAN, EBPServerPresenceSearchType ServerTypeToSearch, const TArray<FSessionsSearchSetting> &Filters, bool bEmptyServersOnly, bool bNonEmptyServersOnly, bool bSecureServersOnly, bool bSearchLobbies, int MinSlotsAvailable)
{
	UFindSessionsCallbackProxyAdvanced* Proxy = NewObject<UFindSessionsCallbackProxyAdvanced>();	
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->bUseLAN = bUseLAN;
	Proxy->MaxResults = MaxResults;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->SearchSettings = Filters;
	Proxy->ServerSearchType = ServerTypeToSearch;
	Proxy->bEmptyServersOnly = bEmptyServersOnly,
	Proxy->bNonEmptyServersOnly = bNonEmptyServersOnly;
	Proxy->bSecureServersOnly = bSecureServersOnly;
	Proxy->bSearchLobbies = bSearchLobbies;
	Proxy->MinSlotsAvailable = MinSlotsAvailable;
	return Proxy;
}

void UFindSessionsCallbackProxyAdvanced::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("FindSessions"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Re-initialize here, otherwise I think there might be issues with people re-calling search for some reason before it is destroyed
			bRunSecondSearch = false;
			bIsOnSecondSearch = false;

			DelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(Delegate);

			SearchObject = MakeShareable(new FOnlineSessionSearch);
			SearchObject->MaxSearchResults = MaxResults;
			SearchObject->bIsLanQuery = bUseLAN;
			//SearchObject->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			// Create temp filter variable, because I had to re-define a blueprint version of this, it is required.
			FOnlineSearchSettingsEx tem;

			/*		// Search only for dedicated servers (value is true/false)
			#define SEARCH_DEDICATED_ONLY FName(TEXT("DEDICATEDONLY"))
			// Search for empty servers only (value is true/false)
			#define SEARCH_EMPTY_SERVERS_ONLY FName(TEXT("EMPTYONLY"))
			// Search for non empty servers only (value is true/false)
			#define SEARCH_NONEMPTY_SERVERS_ONLY FName(TEXT("NONEMPTYONLY"))
			// Search for secure servers only (value is true/false)
			#define SEARCH_SECURE_SERVERS_ONLY FName(TEXT("SECUREONLY"))
			// Search for presence sessions only (value is true/false)
			#define SEARCH_PRESENCE FName(TEXT("PRESENCESEARCH"))
			// Search for a match with min player availability (value is int)
			#define SEARCH_MINSLOTSAVAILABLE FName(TEXT("MINSLOTSAVAILABLE"))
			// Exclude all matches where any unique ids in a given array are present (value is string of the form "uniqueid1;uniqueid2;uniqueid3")
			#define SEARCH_EXCLUDE_UNIQUEIDS FName(TEXT("EXCLUDEUNIQUEIDS"))
			// User ID to search for session of
			#define SEARCH_USER FName(TEXT("SEARCHUSER"))
			// Keywords to match in session search
			#define SEARCH_KEYWORDS FName(TEXT("SEARCHKEYWORDS"))*/
			/** Keywords to match in session search */
			/** The matchmaking queue name to matchmake in, e.g. "TeamDeathmatch" (value is string) */
			/** #define SEARCH_MATCHMAKING_QUEUE FName(TEXT("MATCHMAKINGQUEUE"))*/
			/** If set, use the named Xbox Live hopper to find a session via matchmaking (value is a string) */
			/** #define SEARCH_XBOX_LIVE_HOPPER_NAME FName(TEXT("LIVEHOPPERNAME"))*/
			/** Which session template from the service configuration to use */
			/** #define SEARCH_XBOX_LIVE_SESSION_TEMPLATE_NAME FName(TEXT("LIVESESSIONTEMPLATE"))*/
			/** Selection method used to determine which match to join when multiple are returned (valid only on Switch) */
			/** #define SEARCH_SWITCH_SELECTION_METHOD FName(TEXT("SWITCHSELECTIONMETHOD"))*/
			/** Whether to use lobbies vs sessions */
			/** #define SEARCH_LOBBIES FName(TEXT("LOBBYSEARCH"))*/

			if (bEmptyServersOnly)
				tem.Set(SEARCH_EMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);

			if (bNonEmptyServersOnly)
				tem.Set(SEARCH_NONEMPTY_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);

			if (bSecureServersOnly)
				tem.Set(SEARCH_SECURE_SERVERS_ONLY, true, EOnlineComparisonOp::Equals);

			if (MinSlotsAvailable != 0)
				tem.Set(SEARCH_MINSLOTSAVAILABLE, MinSlotsAvailable, EOnlineComparisonOp::GreaterThanEquals);

			// Filter results
			if (SearchSettings.Num() > 0)
			{
				for (int i = 0; i < SearchSettings.Num(); i++)
				{
					// Function that was added to make directly adding a FVariant possible
					tem.HardSet(SearchSettings[i].PropertyKeyPair.Key, SearchSettings[i].PropertyKeyPair.Data, SearchSettings[i].ComparisonOp);
				}
			}

			switch (ServerSearchType)
			{

			case EBPServerPresenceSearchType::ClientServersOnly:
			{
				tem.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
				
				if (bSearchLobbies && !IOnlineSubsystem::DoesInstanceExist("STEAM"))
					tem.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
			}
			break;

			case EBPServerPresenceSearchType::DedicatedServersOnly:
			{
				//tem.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
			}
			break;

			case EBPServerPresenceSearchType::AllServers:
			default:
			{
				//if (IOnlineSubsystem::DoesInstanceExist("STEAM"))
				//{
				bRunSecondSearch = true;

				SearchObjectDedicated = MakeShareable(new FOnlineSessionSearch);
				SearchObjectDedicated->MaxSearchResults = MaxResults;
				SearchObjectDedicated->bIsLanQuery = bUseLAN;

				FOnlineSearchSettingsEx DedicatedOnly = tem;

				tem.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

				if (bSearchLobbies && !IOnlineSubsystem::DoesInstanceExist("STEAM"))
					tem.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

				//DedicatedOnly.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
				SearchObjectDedicated->QuerySettings = DedicatedOnly;
				//}
			}
			break;
			}

			// Copy the derived temp variable over to it's base class
			SearchObject->QuerySettings = tem;

			Sessions->FindSessions(*Helper.UserID, SearchObject.ToSharedRef());

			// OnQueryCompleted will get called, nothing more to do now
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	// Fail immediately
	OnFailure.Broadcast(SessionSearchResults);
}

void UFindSessionsCallbackProxyAdvanced::OnCompleted(bool bSuccess)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("FindSessionsCallback"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (!Helper.IsValid())
	{
		// Fail immediately
		OnFailure.Broadcast(SessionSearchResults);
		return;
	}

	if (!bRunSecondSearch && Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);
		}
	}

	if (bSuccess)
	{
		if (bIsOnSecondSearch)
		{
			if (SearchObjectDedicated.IsValid())
			{
				// Just log the results for now, will need to add a blueprint-compatible search result struct
				for (auto& Result : SearchObjectDedicated->SearchResults)
				{
					FString ResultText = FString::Printf(TEXT("Found a session. Ping is %d"), Result.PingInMs);

					FFrame::KismetExecutionMessage(*ResultText, ELogVerbosity::Log);

					FBlueprintSessionResult BPResult;
					BPResult.OnlineResult = Result;
					SessionSearchResults.AddUnique(BPResult);
				}
				OnSuccess.Broadcast(SessionSearchResults);
				return;
			}
		}
		else
		{
			if (SearchObject.IsValid())
			{
				// Just log the results for now, will need to add a blueprint-compatible search result struct
				for (auto& Result : SearchObject->SearchResults)
				{
					FString ResultText = FString::Printf(TEXT("Found a session. Ping is %d"), Result.PingInMs);

					FFrame::KismetExecutionMessage(*ResultText, ELogVerbosity::Log);

					FBlueprintSessionResult BPResult;
					BPResult.OnlineResult = Result;
					SessionSearchResults.AddUnique(BPResult);
				}
				if (!bRunSecondSearch)
				{
					OnSuccess.Broadcast(SessionSearchResults);
					return;
				}
			}
		}
	}
	else
	{
		if (!bRunSecondSearch)
		{
			// Need to account for only one of the searches failing
			if (SessionSearchResults.Num() > 0)
				OnSuccess.Broadcast(SessionSearchResults);
			else
				OnFailure.Broadcast(SessionSearchResults);
			return;
		}
	}

	if (Helper.IsValid() && bRunSecondSearch && ServerSearchType == EBPServerPresenceSearchType::AllServers)
	{
		bRunSecondSearch = false;
		bIsOnSecondSearch = true;
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		Sessions->FindSessions(*Helper.UserID, SearchObjectDedicated.ToSharedRef());
	}
	else // We lost our player controller
	{
		if (bSuccess && SessionSearchResults.Num() > 0)
			OnSuccess.Broadcast(SessionSearchResults);
		else
			OnFailure.Broadcast(SessionSearchResults);
	}
}


void UFindSessionsCallbackProxyAdvanced::FilterSessionResults(const TArray<FBlueprintSessionResult> &SessionResults, const TArray<FSessionsSearchSetting> &Filters, TArray<FBlueprintSessionResult> &FilteredResults)
{
	for (int j = 0; j < SessionResults.Num(); j++)
	{
		bool bAddResult = true;

		// Filter results
		if (Filters.Num() > 0)
		{
			const FOnlineSessionSetting * setting;
			for (int i = 0; i < Filters.Num(); i++)
			{
				setting = SessionResults[j].OnlineResult.Session.SessionSettings.Settings.Find(Filters[i].PropertyKeyPair.Key);

				// Couldn't find this key
				if (!setting)
					continue;

				if (!CompareVariants(setting->Data, Filters[i].PropertyKeyPair.Data, Filters[i].ComparisonOp))
				{
					bAddResult = false;
					break;
				}
			}
		}

		if (bAddResult)
			FilteredResults.Add(SessionResults[j]);
	}

	return;
}


bool UFindSessionsCallbackProxyAdvanced::CompareVariants(const FVariantData &A, const FVariantData &B, EOnlineComparisonOpRedux Comparator)
{
	if (A.GetType() != B.GetType())
		return false;

	switch (A.GetType())
	{
	case EOnlineKeyValuePairDataType::Bool:
	{
		bool bA, bB;
		A.GetValue(bA);
		B.GetValue(bB);
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		default:
			return false;break;
		}
	}
	case EOnlineKeyValuePairDataType::Double:
	{
		double bA, bB;
		A.GetValue(bA);
		B.GetValue(bB);
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals:
			return (bA == bB || bA > bB); break;
		case EOnlineComparisonOpRedux::LessThanEquals:
			return (bA == bB || bA < bB); break;
		case EOnlineComparisonOpRedux::GreaterThan:
			return bA > bB; break;
		case EOnlineComparisonOpRedux::LessThan:
			return bA < bB; break;
		default:
			return false; break;
		}
	}
	case EOnlineKeyValuePairDataType::Float:
	{
		float tbA, tbB;
		double bA, bB;
		A.GetValue(tbA);
		B.GetValue(tbB);
		bA = (double)tbA;
		bB = (double)tbB;
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals:
			return (bA == bB || bA > bB); break;
		case EOnlineComparisonOpRedux::LessThanEquals:
			return (bA == bB || bA < bB); break;
		case EOnlineComparisonOpRedux::GreaterThan:
			return bA > bB; break;
		case EOnlineComparisonOpRedux::LessThan:
			return bA < bB; break;
		default:
			return false; break;
		}
	}
	case EOnlineKeyValuePairDataType::Int32:
	{
		int32 bA, bB;
		A.GetValue(bA);
		B.GetValue(bB);
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals:
			return (bA == bB || bA > bB); break;
		case EOnlineComparisonOpRedux::LessThanEquals:
			return (bA == bB || bA < bB); break;
		case EOnlineComparisonOpRedux::GreaterThan:
			return bA > bB; break;
		case EOnlineComparisonOpRedux::LessThan:
			return bA < bB; break;
		default:
			return false; break;
		}
	}
	case EOnlineKeyValuePairDataType::Int64:
	{
		uint64 bA, bB;
		A.GetValue(bA);
		B.GetValue(bB);
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		case EOnlineComparisonOpRedux::GreaterThanEquals:
			return (bA == bB || bA > bB); break;
		case EOnlineComparisonOpRedux::LessThanEquals:
			return (bA == bB || bA < bB); break;
		case EOnlineComparisonOpRedux::GreaterThan:
			return bA > bB; break;
		case EOnlineComparisonOpRedux::LessThan:
			return bA < bB; break;
		default:
			return false; break;
		}
	}

	case EOnlineKeyValuePairDataType::String:
	{
		FString bA, bB;
		A.GetValue(bA);
		B.GetValue(bB);
		switch (Comparator)
		{
		case EOnlineComparisonOpRedux::Equals:
			return bA == bB; break;
		case EOnlineComparisonOpRedux::NotEquals:
			return bA != bB; break;
		default:
			return false; break;
		}
	}

	case EOnlineKeyValuePairDataType::Empty:
	case EOnlineKeyValuePairDataType::Blob:
	default:
		return false; break;
	}



}