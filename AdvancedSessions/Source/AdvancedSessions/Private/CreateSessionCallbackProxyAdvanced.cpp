// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#include "CreateSessionCallbackProxyAdvanced.h"


//////////////////////////////////////////////////////////////////////////
// UCreateSessionCallbackProxyAdvanced

UCreateSessionCallbackProxyAdvanced::UCreateSessionCallbackProxyAdvanced(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CreateCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateCompleted))
	, StartCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartCompleted))
	, NumPublicConnections(1)
{
}

UCreateSessionCallbackProxyAdvanced* UCreateSessionCallbackProxyAdvanced::CreateAdvancedSession(UObject* WorldContextObject, const TArray<FSessionPropertyKeyPair>& ExtraSettings, class APlayerController* PlayerController, int32 PublicConnections, int32 PrivateConnections, bool bUseLAN, bool bAllowInvites, bool bIsDedicatedServer, /*bool bUsePresence,*/ bool bUseLobbiesIfAvailable, bool bAllowJoinViaPresence, bool bAllowJoinViaPresenceFriendsOnly, bool bAntiCheatProtected, bool bUsesStats, bool bShouldAdvertise, bool bUseLobbiesVoiceChatIfAvailable, bool bStartAfterCreate)
{
	UCreateSessionCallbackProxyAdvanced* Proxy = NewObject<UCreateSessionCallbackProxyAdvanced>();
	Proxy->PlayerControllerWeakPtr = PlayerController;
	Proxy->NumPublicConnections = PublicConnections;
	Proxy->NumPrivateConnections = PrivateConnections;
	Proxy->bUseLAN = bUseLAN;
	Proxy->WorldContextObject = WorldContextObject;
	Proxy->bAllowInvites = bAllowInvites;
	Proxy->ExtraSettings = ExtraSettings;
	Proxy->bDedicatedServer = bIsDedicatedServer;
	/*Proxy->bUsePresence = bUsePresence;*/
	Proxy->bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	Proxy->bAllowJoinViaPresence = bAllowJoinViaPresence;
	Proxy->bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
	Proxy->bAntiCheatProtected = bAntiCheatProtected;
	Proxy->bUsesStats = bUsesStats;
	Proxy->bShouldAdvertise = bShouldAdvertise;
	Proxy->bUseLobbiesVoiceChatIfAvailable = bUseLobbiesVoiceChatIfAvailable;
	Proxy->bStartAfterCreate = bStartAfterCreate;
	return Proxy;
}

void UCreateSessionCallbackProxyAdvanced::Activate()
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CreateSession"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	
	if (PlayerControllerWeakPtr.IsValid() )
		Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.OnlineSub != nullptr)
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			CreateCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegate);
			
			FOnlineSessionSettings Settings;
			Settings.NumPublicConnections = NumPublicConnections;
			Settings.NumPrivateConnections = NumPrivateConnections;
			Settings.bShouldAdvertise = bShouldAdvertise;
			Settings.bAllowJoinInProgress = true;
			Settings.bIsLANMatch = bUseLAN;
			Settings.bAllowJoinViaPresence = bAllowJoinViaPresence;
			Settings.bIsDedicated = bDedicatedServer;

			if (bDedicatedServer)
			{
				Settings.bUseLobbiesIfAvailable = false;
				Settings.bUsesPresence = false;
			}
			else
			{
				Settings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
				Settings.bUsesPresence = bUseLobbiesIfAvailable;
			}

			Settings.bUseLobbiesVoiceChatIfAvailable = bUseLobbiesIfAvailable ? bUseLobbiesVoiceChatIfAvailable : false;
			Settings.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
			Settings.bAntiCheatProtected = bAntiCheatProtected;
			Settings.bUsesStats = bUsesStats;

			// These are about the only changes over the standard Create Sessions Node
			Settings.bAllowInvites = bAllowInvites;
			
			FOnlineSessionSetting ExtraSetting;
			for (int i = 0; i < ExtraSettings.Num(); i++)
			{
				ExtraSetting.Data = ExtraSettings[i].Data;
				//	ViaOnlineServiceAndPing
				ExtraSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
				Settings.Settings.Add(ExtraSettings[i].Key, ExtraSetting);
			}
			
			
			if (!bDedicatedServer )
			{
				if (PlayerControllerWeakPtr.IsValid() && Helper.UserID.IsValid())
				{
					Sessions->CreateSession(*Helper.UserID, NAME_GameSession, Settings);
				}
				else
				{
					FFrame::KismetExecutionMessage(TEXT("Invalid Player controller when attempting to start a session"), ELogVerbosity::Warning);
					Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
					
					// Fail immediately
					OnFailure.Broadcast();
				}
			}
			else
				Sessions->CreateSession(0, NAME_GameSession, Settings);

			// OnCreateCompleted will get called, nothing more to do now
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

void UCreateSessionCallbackProxyAdvanced::OnCreateCompleted(FName SessionName, bool bWasSuccessful)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("CreateSessionCallback"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	//Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.OnlineSub != nullptr)
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
			
			if (bWasSuccessful)
			{
				if (this->bStartAfterCreate)
				{
					UE_LOG_ONLINE_SESSION(Display, TEXT("Session creation completed. Automatic start is turned on, starting session now."));
					StartCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(StartCompleteDelegate);
					Sessions->StartSession(NAME_GameSession); // We'll call `OnSuccess.Broadcast()` when start succeeds.
				}
				else
				{
					UE_LOG_ONLINE_SESSION(Display, TEXT("Session creation completed. Automatic start is turned off, to start the session call 'StartSession'."));
					OnSuccess.Broadcast();
				}

				// OnStartCompleted will get called, nothing more to do now
				return;
			}
		}
	}

	if (!bWasSuccessful)
	{
		OnFailure.Broadcast();
	}
}

void UCreateSessionCallbackProxyAdvanced::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	FOnlineSubsystemBPCallHelperAdvanced Helper(TEXT("StartSessionCallback"), GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull));
	//Helper.QueryIDFromPlayerController(PlayerControllerWeakPtr.Get());

	if (Helper.OnlineSub != nullptr)
	{
		auto Sessions = Helper.OnlineSub->GetSessionInterface();
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
