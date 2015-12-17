// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "OnlineSubSystemHeader.h"
#include "FindSessionsCallbackProxyAdvanced.h"

//////////////////////////////////////////////////////////////////////////
// UFindSessionsCallbackProxyAdvanced


UFindSessionsCallbackProxyAdvanced::UFindSessionsCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
	, bUseLAN(false)
{
}

UFindSessionsCallbackProxyAdvanced* UFindSessionsCallbackProxyAdvanced::FindSessionsAdvanced(UObject* WorldContextObject, class APlayerController* PlayerController, int MaxResults, bool bUseLAN, const TArray<FSessionsSearchSetting> &Filters)
{
	UFindSessionsCallbackProxyAdvanced* Proxy = NewObject<UFindSessionsCallbackProxyAdvanced>();	
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->bUseLAN = bUseLAN;
	Proxy->MaxResults = MaxResults;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->SearchSettings = Filters;
	return Proxy;
}

void UFindSessionsCallbackProxyAdvanced::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("FindSessions"), GEngine->GetWorldFromContextObject(WorldContextObject));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			DelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(Delegate);
			
			SearchObject = MakeShareable(new FOnlineSessionSearch);
			SearchObject->MaxSearchResults = MaxResults;
			SearchObject->bIsLanQuery = bUseLAN;
			//SearchObject->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			// Create temp filter variable, because I had to re-define a blueprint version of this, it is required.
			FOnlineSearchSettingsEx tem;
			tem.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			// Filter results
			if (SearchSettings.Num() > 0)
			{
				for (int i = 0; i < SearchSettings.Num(); i++)
				{
					// Function that was added to make directly adding a FVariant possible
					tem.HardSet(SearchSettings[i].PropertyKeyPair.Key, SearchSettings[i].PropertyKeyPair.Data, SearchSettings[i].ComparisonOp);
				}
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
	TArray<FBlueprintSessionResult> Results;
	OnFailure.Broadcast(Results);
}

void UFindSessionsCallbackProxyAdvanced::OnCompleted(bool bSuccess)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("FindSessionsCallback"), GEngine->GetWorldFromContextObject(WorldContextObject));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);
		}
	}

	TArray<FBlueprintSessionResult> Results;

	if (bSuccess && SearchObject.IsValid())
	{
		// Just log the results for now, will need to add a blueprint-compatible search result struct
		for (auto& Result : SearchObject->SearchResults)
		{
		/*	bool bAddResult = true;

			// Filter results
			if (SearchSettings.Num() > 0)
			{
				FOnlineSessionSetting * setting;
				for (int i = 0; i < SearchSettings.Num(); i++)
				{
					setting = Result.Session.SessionSettings.Settings.Find(SearchSettings[i].PropertyKeyPair.Key);

					// Couldn't find this key
					if (!setting)
						continue;

					if (!CompareVariants(setting->Data, SearchSettings[i].PropertyKeyPair.Data, SearchSettings[i].ComparisonOp))
					{
						bAddResult = false;
						break;
					}
				}
			}*/

			//if (bAddResult)
			//{
				FString ResultText = FString::Printf(TEXT("Found a session. Ping is %d"), Result.PingInMs);

				FFrame::KismetExecutionMessage(*ResultText, ELogVerbosity::Log);

				FBlueprintSessionResult BPResult;
				BPResult.OnlineResult = Result;
				Results.Add(BPResult);
			//}
		}
		OnSuccess.Broadcast(Results);
	}
	else
	{
		OnFailure.Broadcast(Results);
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


bool UFindSessionsCallbackProxyAdvanced::CompareVariants(const FVariantData &A, const FVariantData &B, EOnlineComparisonOpRedux::Type Comparator)
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