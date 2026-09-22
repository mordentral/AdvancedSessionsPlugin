#include "StartMatchmakingCallbackProxy.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

UStartMatchmakingCallbackProxy* UStartMatchmakingCallbackProxy::StartMatchmaking(UObject* WorldContextObject, APlayerController* PlayerController, const TArray<FSessionsSearchSetting>& Filters)
{
	auto* Proxy = NewObject<UStartMatchmakingCallbackProxy>();
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->Filters = Filters;
	return Proxy;
}

void UStartMatchmakingCallbackProxy::Activate()
{
	auto* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull);

	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("StartMatchmakingCallbackProxy"), World);
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (World && Helper.IsValid())
	{
		if (const auto& SessionInt = Helper.OnlineSub->GetSessionInterface())
		{
			auto OnlineSearch = MakeShared<FOnlineSessionSearch>();

			FOnlineSearchSettingsEx SearchSettings;
			for (const auto& Filter : Filters)
			{
				SearchSettings.HardSet(Filter.PropertyKeyPair.Key, Filter.PropertyKeyPair.Data, Filter.ComparisonOp);
			}
			OnlineSearch->QuerySettings = SearchSettings;

			TArray<TSharedRef<const FUniqueNetId>> LocalPlayers;
			for (const auto* LocalPlayer : World->GetGameInstance()->GetLocalPlayers())
			{
				if (const auto& UniqueNetId = LocalPlayer->GetPreferredUniqueNetId(); UniqueNetId.IsValid())
				{
					LocalPlayers.Add(UniqueNetId->AsShared());
				}
			}

			// TODO: What's the purpose of this?
			const static FOnlineSessionSettings Dummy;

			DelegateHandle = SessionInt->AddOnMatchmakingCompleteDelegate_Handle(FOnMatchmakingCompleteDelegate::CreateUObject(this, &ThisClass::OnMatchmakingComplete));
			SessionInt->StartMatchmaking(LocalPlayers, NAME_GameSession, Dummy, OnlineSearch);
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	OnFailure.Broadcast();
}

void UStartMatchmakingCallbackProxy::OnMatchmakingComplete(const FName SessionName, const bool bWasSuccessful)
{
	if (const auto* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull))
	{
		if (const auto& SessionInt = Online::GetSessionInterface(World))
		{
			SessionInt->ClearOnMatchmakingCompleteDelegate_Handle(DelegateHandle);

			if (bWasSuccessful && PlayerControllerWeakPtr.IsValid())
			{
				if (FString ConnectString; SessionInt->GetResolvedConnectString(SessionName, ConnectString))
				{
					UE_LOG_ONLINE_SESSION(Log, TEXT("Matchmaking: traveling to %s"), *ConnectString);
					PlayerControllerWeakPtr->ClientTravel(ConnectString, TRAVEL_Absolute);
					OnSuccess.Broadcast();
					return;
				}
			}
		}
	}

	OnFailure.Broadcast();
}
