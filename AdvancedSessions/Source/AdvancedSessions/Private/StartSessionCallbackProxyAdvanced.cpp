#include "StartSessionCallbackProxyAdvanced.h"

UStartSessionCallbackProxyAdvanced::UStartSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	  , StartCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartCompleted))
{
}

UStartSessionCallbackProxyAdvanced* UStartSessionCallbackProxyAdvanced::StartAdvancedSession(
	const UObject* WorldContextObject)
{
	UStartSessionCallbackProxyAdvanced* Proxy = NewObject<UStartSessionCallbackProxyAdvanced>();
	Proxy->WorldContextObject = WorldContextObject;
	return Proxy;
}

void UStartSessionCallbackProxyAdvanced::Activate()
{
	const FOnlineSubsystemBPCallHelperAdvanced Helper(
		TEXT("StartSession"),
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));

	if (Helper.OnlineSub != nullptr)
	{
		const auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			StartCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(StartCompleteDelegate);
			Sessions->StartSession(NAME_GameSession);
			return;
		}
		FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
	}

	// Fail immediately
	OnFailure.Broadcast();
}

void UStartSessionCallbackProxyAdvanced::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	const FOnlineSubsystemBPCallHelperAdvanced Helper(
		TEXT("StartSessionCallback"),
		GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull));

	if (Helper.OnlineSub != nullptr)
	{
		const auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(StartCompleteDelegateHandle);
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
