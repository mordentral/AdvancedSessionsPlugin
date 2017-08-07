// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EndSessionCallbackProxy.h"


//////////////////////////////////////////////////////////////////////////
// UEndSessionCallbackProxy

UEndSessionCallbackProxy::UEndSessionCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnEndSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UEndSessionCallbackProxy* UEndSessionCallbackProxy::EndSession(UObject* WorldContextObject, class APlayerController* PlayerController)
{
	UEndSessionCallbackProxy* Proxy = NewObject<UEndSessionCallbackProxy>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UEndSessionCallbackProxy::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("EndSession"), GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			FNamedOnlineSession* Session = Sessions->GetNamedSession(GameSessionName);
			if (Session &&
				Session->SessionState == EOnlineSessionState::InProgress)
			{
				DelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(Delegate);
				Sessions->EndSession(GameSessionName);
			}
			else
			{
				OnSuccess.Broadcast();
			}
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

void UEndSessionCallbackProxy::OnCompleted(FName SessionName, bool bWasSuccessful)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("EndSessionCallback"), GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));
	Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.IsValid())
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(DelegateHandle);
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
