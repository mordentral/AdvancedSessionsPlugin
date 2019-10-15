// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "CancelFindSessionsCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// UCancelFindSessionsCallbackProxy

UCancelFindSessionsCallbackProxy::UCancelFindSessionsCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnCancelFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UCancelFindSessionsCallbackProxy* UCancelFindSessionsCallbackProxy::CancelFindSessions(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	UCancelFindSessionsCallbackProxy* Proxy = NewObject<UCancelFindSessionsCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UCancelFindSessionsCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CancelFindSessions"), GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())		
		{
			DelegateHandle = Sessions->AddOnCancelFindSessionsCompleteDelegate_Handle(Delegate);
			Sessions->CancelFindSessions();

			// OnCompleted will get called, nothing more to do now
			return;
		}
		else
		{
			FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
		}
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UCancelFindSessionsCallbackProxy::OnCompleted(bool bWasSuccessful)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CancelFindSessionsCallback"), GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCancelFindSessionsCompleteDelegate_Handle(DelegateHandle);
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
