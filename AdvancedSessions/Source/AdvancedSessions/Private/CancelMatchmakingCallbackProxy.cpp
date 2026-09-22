#include "CancelMatchmakingCallbackProxy.h"

UCancelMatchmakingCallbackProxy* UCancelMatchmakingCallbackProxy::CancelMatchmaking(UObject* WorldContextObject, APlayerController* PlayerController)
{
	auto* Proxy = NewObject<UCancelMatchmakingCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UCancelMatchmakingCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CancelMatchmakingCallbackProxy"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		if (const auto& Sessions = Helper.OnlineSub->GetSessionInterface())
		{
			DelegateHandle = Sessions->AddOnCancelMatchmakingCompleteDelegate_Handle(FOnMatchmakingCompleteDelegate::CreateUObject(this, &ThisClass::OnCancelMatchmakingComplete));
			Sessions->CancelMatchmaking(*Helper.UserID, NAME_GameSession);
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	OnFailure.Broadcast();
}

void UCancelMatchmakingCallbackProxy::OnCancelMatchmakingComplete(const FName SessionName, const bool bWasSuccessful)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CancelMatchmakingCallbackProxy"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		if (const auto& Sessions = Helper.OnlineSub->GetSessionInterface())
		{
			Sessions->ClearOnCancelMatchmakingCompleteDelegate_Handle(DelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		OnSuccess.Broadcast();
	}
	else
	{
		OnFailure.Broadcast();
	}
}
