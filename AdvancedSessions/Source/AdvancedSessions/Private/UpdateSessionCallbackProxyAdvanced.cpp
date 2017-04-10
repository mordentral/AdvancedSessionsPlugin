// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "UpdateSessionCallbackProxyAdvanced.h"


//////////////////////////////////////////////////////////////////////////
// UUpdateSessionCallbackProxyAdvanced

UUpdateSessionCallbackProxyAdvanced::UUpdateSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnUpdateSessionCompleteDelegate(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnUpdateCompleted))
	, NumPublicConnections(1)
{
}	

UUpdateSessionCallbackProxyAdvanced* UUpdateSessionCallbackProxyAdvanced::UpdateSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair> &ExtraSettings, int32 PublicConnections, int32 PrivateConnections, bool bUseLAN, bool bAllowInvites, bool bAllowJoinInProgress, bool bRefreshOnlineData, bool bIsDedicatedServer)
{
	UUpdateSessionCallbackProxyAdvanced* Proxy = NewObject<UUpdateSessionCallbackProxyAdvanced>();
	Proxy->NumPublicConnections = PublicConnections;
	Proxy->NumPrivateConnections = PrivateConnections;
	Proxy->bUseLAN = bUseLAN;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->bAllowInvites = bAllowInvites;
	Proxy->ExtraSettings = ExtraSettings;
	Proxy->bRefreshOnlineData = bRefreshOnlineData;
	Proxy->bAllowJoinInProgress = bAllowJoinInProgress;
	Proxy->bDedicatedServer = bIsDedicatedServer;
	return Proxy;	
}

void UUpdateSessionCallbackProxyAdvanced::Activate()
{

	IOnlineSessionPtr Sessions = Online::GetSessionInterface();

	if (Sessions.IsValid())
	{
		if (Sessions->GetNumSessions() < 1)
		{
			OnFailure.Broadcast();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NO REGISTERED SESSIONS!"));
			return;
		}

		// This gets the actual session itself
		//FNamedOnlineSession * curSession = Sessions->GetNamedSession(GameSessionName);
		FOnlineSessionSettings* Settings = Sessions->GetSessionSettings(GameSessionName);

		if (!Settings)
		{
			// Fail immediately
			OnFailure.Broadcast();
			return;
		}

		OnUpdateSessionCompleteDelegateHandle = Sessions->AddOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteDelegate);

	//	FOnlineSessionSettings Settings;
		//Settings->BuildUniqueId = GetBuildUniqueId();
		Settings->NumPublicConnections = NumPublicConnections;
		Settings->NumPrivateConnections = NumPrivateConnections;
		//Settings->bShouldAdvertise = true;
		Settings->bAllowJoinInProgress = bAllowJoinInProgress;
		Settings->bIsLANMatch = bUseLAN;
		//Settings->bUsesPresence = true;
		//Settings->bAllowJoinViaPresence = true;
		Settings->bAllowInvites = bAllowInvites;
		Settings->bAllowJoinInProgress = bAllowJoinInProgress;
		Settings->bIsDedicated = bDedicatedServer;

		FOnlineSessionSetting * fSetting = NULL;
		FOnlineSessionSetting ExtraSetting;
		for (int i = 0; i < ExtraSettings.Num(); i++)
		{
			fSetting = Settings->Settings.Find(ExtraSettings[i].Key);

			if (fSetting)
			{
				fSetting->Data = ExtraSettings[i].Data;
			}
			else
			{
				ExtraSetting.Data = ExtraSettings[i].Data;
				ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
				Settings->Settings.Add(ExtraSettings[i].Key, ExtraSetting);
			}
		}

		Sessions->UpdateSession(GameSessionName, *Settings, bRefreshOnlineData);

		// OnUpdateCompleted will get called, nothing more to do now
		return;
	}
	else
	{
		FFrame::KismetExecutionMessage(TEXT("Sessions not supported by Online Subsystem"), ELogVerbosity::Warning);
	}
	// Fail immediately
	OnFailure.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sessions not supported"));
}

void UUpdateSessionCallbackProxyAdvanced::OnUpdateCompleted(FName SessionName, bool bWasSuccessful)
{
	IOnlineSessionPtr Sessions = Online::GetSessionInterface();
	if (Sessions.IsValid())
	{
		Sessions->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteDelegateHandle);
			
		if (bWasSuccessful)
		{
			OnSuccess.Broadcast();
			return;
		}
	}

	if (!bWasSuccessful)
	{
		OnFailure.Broadcast();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WAS NOT SUCCESSFUL"));
	}
}