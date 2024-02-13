// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedSessionsLibrary.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

//General Log
DEFINE_LOG_CATEGORY(AdvancedSessionsLog);


bool UAdvancedSessionsLibrary::KickPlayer(UObject* WorldContextObject, APlayerController* PlayerToKick, FText KickReason)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World)
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (GameMode->GameSession)
			{
				return GameMode->GameSession->KickPlayer(PlayerToKick, KickReason);
			}
		}
	}

	return false;
}

bool UAdvancedSessionsLibrary::BanPlayer(UObject* WorldContextObject, APlayerController* PlayerToBan, FText BanReason)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (World)
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			if (GameMode->GameSession)
			{
				return GameMode->GameSession->BanPlayer(PlayerToBan, BanReason);
			}
		}
	}

	return false;
}

bool UAdvancedSessionsLibrary::IsValidSession(const FBlueprintSessionResult & SessionResult)
{
	return SessionResult.OnlineResult.IsValid();
}

void UAdvancedSessionsLibrary::GetSessionID_AsString(const FBlueprintSessionResult & SessionResult, FString& SessionID)
{
	const TSharedPtr<class FOnlineSessionInfo> SessionInfo = SessionResult.OnlineResult.Session.SessionInfo;
	if (SessionInfo.IsValid() && SessionInfo->IsValid() && SessionInfo->GetSessionId().IsValid())
	{
		SessionID = SessionInfo->GetSessionId().ToString();
		return;
	}

	// Zero the string out if we didn't have a valid one, in case this is called in c++
	SessionID.Empty();
}

void UAdvancedSessionsLibrary::GetCurrentSessionID_AsString(UObject* WorldContextObject, FString& SessionID)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);

	if (!SessionInterface.IsValid()) 
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetCurrentSessionID_AsString couldn't get the session interface!"));
		SessionID.Empty();
		return;
	}

	const FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
	if (Session != nullptr) 
	{
		const TSharedPtr<class FOnlineSessionInfo> SessionInfo = Session->SessionInfo;
		if (SessionInfo.IsValid() && SessionInfo->IsValid() && SessionInfo->GetSessionId().IsValid()) 
		{
			SessionID = SessionInfo->GetSessionId().ToString();
			return;
		}
	}

	// Zero the string out if we didn't have a valid one, in case this is called in c++
	SessionID.Empty();
}

void UAdvancedSessionsLibrary::GetCurrentUniqueBuildID(int32 &UniqueBuildId)
{
	UniqueBuildId = GetBuildUniqueId();
}

void UAdvancedSessionsLibrary::GetUniqueBuildID(FBlueprintSessionResult SessionResult, int32 &UniqueBuildId)
{
	UniqueBuildId = SessionResult.OnlineResult.Session.SessionSettings.BuildUniqueId;
}

FName UAdvancedSessionsLibrary::GetSessionPropertyKey(const FSessionPropertyKeyPair& SessionProperty)
{
	return SessionProperty.Key;
}

void UAdvancedSessionsLibrary::FindSessionPropertyByName(const TArray<FSessionPropertyKeyPair>& ExtraSettings, FName SettingName, EBlueprintResultSwitch &Result, FSessionPropertyKeyPair& OutProperty)
{
	const FSessionPropertyKeyPair* prop = ExtraSettings.FindByPredicate([&](const FSessionPropertyKeyPair& it) {return it.Key == SettingName; });
	if (prop)
	{
		Result = EBlueprintResultSwitch::OnSuccess;
		OutProperty = *prop;
		return;
	}

	Result = EBlueprintResultSwitch::OnFailure;
}

void UAdvancedSessionsLibrary::FindSessionPropertyIndexByName(const TArray<FSessionPropertyKeyPair>& ExtraSettings, FName SettingName, EBlueprintResultSwitch &Result, int32& OutIndex)
{
	OutIndex = ExtraSettings.IndexOfByPredicate([&](const FSessionPropertyKeyPair& it) {return it.Key == SettingName; });

	Result = OutIndex != INDEX_NONE ? EBlueprintResultSwitch::OnSuccess : EBlueprintResultSwitch::OnFailure;
}	

void UAdvancedSessionsLibrary::AddOrModifyExtraSettings(UPARAM(ref) TArray<FSessionPropertyKeyPair> & SettingsArray, UPARAM(ref) TArray<FSessionPropertyKeyPair> & NewOrChangedSettings, TArray<FSessionPropertyKeyPair> & ModifiedSettingsArray)
{
	ModifiedSettingsArray = SettingsArray;

	bool bFoundSetting = false;
	// For each new setting
	for (const FSessionPropertyKeyPair& Setting : NewOrChangedSettings)
	{
		bFoundSetting = false;

		for (FSessionPropertyKeyPair & itr : ModifiedSettingsArray)
		{
			// Manually comparing the keys
			if (itr.Key == Setting.Key)
			{
				bFoundSetting = true;
				itr.Data = Setting.Data;
			}
		}

		// If it was not found, add to the array instead
		if (!bFoundSetting)
		{
			ModifiedSettingsArray.Add(Setting);
		}
	}

}

void UAdvancedSessionsLibrary::GetExtraSettings(FBlueprintSessionResult SessionResult, TArray<FSessionPropertyKeyPair> & ExtraSettings)
{
	FSessionPropertyKeyPair NewSetting;
	for (auto& Elem : SessionResult.OnlineResult.Session.SessionSettings.Settings)
	{
		NewSetting.Key = Elem.Key;
		NewSetting.Data = Elem.Value.Data;
		ExtraSettings.Add(NewSetting);
	}
}

void UAdvancedSessionsLibrary::GetSessionState(UObject* WorldContextObject, EBPOnlineSessionState &SessionState)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetSessionState couldn't get the session interface!"));
		return;
	}

	SessionState = ((EBPOnlineSessionState)SessionInterface->GetSessionState(NAME_GameSession));
}

void UAdvancedSessionsLibrary::GetSessionSettings(UObject* WorldContextObject, int32 &NumConnections, int32 &NumPrivateConnections, bool &bIsLAN, bool &bIsDedicated, bool &bAllowInvites, bool &bAllowJoinInProgress, bool &bIsAnticheatEnabled, int32 &BuildUniqueID, TArray<FSessionPropertyKeyPair> &ExtraSettings, EBlueprintResultSwitch &Result)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetSessionSettings couldn't get the session interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	FOnlineSessionSettings* settings = SessionInterface->GetSessionSettings(NAME_GameSession);
	if (!settings)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetSessionSettings couldn't get the session settings!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	BuildUniqueID = settings->BuildUniqueId;
	NumConnections = settings->NumPublicConnections;
	NumPrivateConnections = settings->NumPrivateConnections;
	bIsLAN = settings->bIsLANMatch;
	bIsDedicated = settings->bIsDedicated;
	bIsAnticheatEnabled = settings->bAntiCheatProtected;
	bAllowInvites = settings->bAllowInvites;
	bAllowJoinInProgress = settings->bAllowJoinInProgress;

	FSessionPropertyKeyPair NewSetting;

	for (auto& Elem : settings->Settings)
	{
		NewSetting.Key = Elem.Key;
		NewSetting.Data = Elem.Value.Data;
		ExtraSettings.Add(NewSetting);
	}

	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedSessionsLibrary::IsPlayerInSession(UObject* WorldContextObject, const FBPUniqueNetId &PlayerToCheck, bool &bIsInSession)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("IsPlayerInSession couldn't get the session interface!"));
		bIsInSession = false;
		return;
	}

	bIsInSession = SessionInterface->IsPlayerInSession(NAME_GameSession, *PlayerToCheck.GetUniqueNetId());
}

FSessionsSearchSetting UAdvancedSessionsLibrary::MakeLiteralSessionSearchProperty(FSessionPropertyKeyPair SessionSearchProperty, EOnlineComparisonOpRedux ComparisonOp)
{
	FSessionsSearchSetting setting;
	setting.PropertyKeyPair = SessionSearchProperty;
	setting.ComparisonOp = ComparisonOp;

	return setting;
}

FSessionPropertyKeyPair UAdvancedSessionsLibrary::MakeLiteralSessionPropertyByte(FName Key, uint8 Value)
{
	FSessionPropertyKeyPair Prop;
	Prop.Key = Key;
	Prop.Data.SetValue((int32)Value);
	return Prop;
}

FSessionPropertyKeyPair UAdvancedSessionsLibrary::MakeLiteralSessionPropertyBool(FName Key, bool Value)
{
	FSessionPropertyKeyPair Prop;
	Prop.Key = Key;
	Prop.Data.SetValue(Value);
	return Prop;
}

FSessionPropertyKeyPair UAdvancedSessionsLibrary::MakeLiteralSessionPropertyString(FName Key, FString Value)
{
	FSessionPropertyKeyPair Prop;
	Prop.Key = Key;
	Prop.Data.SetValue(Value);
	return Prop;
}

FSessionPropertyKeyPair UAdvancedSessionsLibrary::MakeLiteralSessionPropertyInt(FName Key, int32 Value)
{
	FSessionPropertyKeyPair Prop;
	Prop.Key = Key;
	Prop.Data.SetValue(Value);
	return Prop;
}

FSessionPropertyKeyPair UAdvancedSessionsLibrary::MakeLiteralSessionPropertyFloat(FName Key, float Value)
{
	FSessionPropertyKeyPair Prop;
	Prop.Key = Key;
	Prop.Data.SetValue(Value);
	return Prop;
}

void UAdvancedSessionsLibrary::GetSessionPropertyByte(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, uint8 &SettingValue)
{
	for (FSessionPropertyKeyPair itr : ExtraSettings)
	{
		if (itr.Key == SettingName)
		{
			if (itr.Data.GetType() == EOnlineKeyValuePairDataType::Int32)
			{
				int32 Val;
				itr.Data.GetValue(Val);
				SettingValue = (uint8)(Val);
				SearchResult = ESessionSettingSearchResult::Found;
			}
			else
			{
				SearchResult = ESessionSettingSearchResult::WrongType;
			}
			return;
		}
	}

	SearchResult = ESessionSettingSearchResult::NotFound;
	return;
}

void UAdvancedSessionsLibrary::GetSessionPropertyBool(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, bool &SettingValue)
{
	for (FSessionPropertyKeyPair itr : ExtraSettings)
	{
		if (itr.Key == SettingName)
		{
			if (itr.Data.GetType() == EOnlineKeyValuePairDataType::Bool)
			{
				itr.Data.GetValue(SettingValue);
				SearchResult = ESessionSettingSearchResult::Found;
			}
			else
			{
				SearchResult = ESessionSettingSearchResult::WrongType;
			}
			return;
		}
	}

	SearchResult = ESessionSettingSearchResult::NotFound;
	return;
}

void UAdvancedSessionsLibrary::GetSessionPropertyString(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, FString &SettingValue)
{
	for (FSessionPropertyKeyPair itr : ExtraSettings)
	{
		if (itr.Key == SettingName)
		{
			if (itr.Data.GetType() == EOnlineKeyValuePairDataType::String)
			{
				itr.Data.GetValue(SettingValue);
				SearchResult = ESessionSettingSearchResult::Found;
			}
			else
			{
				SearchResult = ESessionSettingSearchResult::WrongType;
			}
			return;
		}
	}

	SearchResult = ESessionSettingSearchResult::NotFound;
	return;
}

void UAdvancedSessionsLibrary::GetSessionPropertyInt(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, int32 &SettingValue)
{
	for (FSessionPropertyKeyPair itr : ExtraSettings)
	{
		if (itr.Key == SettingName)
		{
			if (itr.Data.GetType() == EOnlineKeyValuePairDataType::Int32)
			{
				itr.Data.GetValue(SettingValue);
				SearchResult = ESessionSettingSearchResult::Found;
			}
			else
			{
				SearchResult = ESessionSettingSearchResult::WrongType;
			}
			return;
		}
	}

	SearchResult = ESessionSettingSearchResult::NotFound;
	return;
}

void UAdvancedSessionsLibrary::GetSessionPropertyFloat(const TArray<FSessionPropertyKeyPair> & ExtraSettings, FName SettingName, ESessionSettingSearchResult &SearchResult, float &SettingValue)
{
	for (FSessionPropertyKeyPair itr : ExtraSettings)
	{
		if (itr.Key == SettingName)
		{
			if (itr.Data.GetType() == EOnlineKeyValuePairDataType::Float)
			{
				itr.Data.GetValue(SettingValue);
				SearchResult = ESessionSettingSearchResult::Found;
			}
			else
			{
				SearchResult = ESessionSettingSearchResult::WrongType;
			}
			return;
		}
	}

	SearchResult = ESessionSettingSearchResult::NotFound;
	return;
}


bool UAdvancedSessionsLibrary::HasOnlineSubsystem(FName SubSystemName)
{
	return IOnlineSubsystem::DoesInstanceExist(SubSystemName);
}

void UAdvancedSessionsLibrary::GetNetPlayerIndex(APlayerController *PlayerController, int32 &NetPlayerIndex)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetNetPlayerIndex received a bad PlayerController!"));
		NetPlayerIndex = 0;
		return;
	}

	NetPlayerIndex = PlayerController->NetPlayerIndex;
	return;
}

void UAdvancedSessionsLibrary::UniqueNetIdToString(const FBPUniqueNetId& UniqueNetId, FString &String)
{
	const FUniqueNetId * ID = UniqueNetId.GetUniqueNetId();

	if ( !ID )
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("UniqueNetIdToString received a bad UniqueNetId!"));
		String = "ERROR, BAD UNIQUE NET ID";
	}
	else
		String = ID->ToString();
}


void UAdvancedSessionsLibrary::GetUniqueNetID(APlayerController *PlayerController, FBPUniqueNetId &UniqueNetId)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetUniqueNetIdFromController received a bad PlayerController!"));
		return;
	}

	if (APlayerState* PlayerState = (PlayerController != NULL) ? PlayerController->PlayerState : NULL)
	{
		UniqueNetId.SetUniqueNetId(PlayerState->GetUniqueId().GetUniqueNetId());
		if (!UniqueNetId.IsValid())
		{
			UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetUniqueNetIdFromController couldn't get the player uniquenetid!"));
		}
		return;
	}
}

void UAdvancedSessionsLibrary::GetUniqueNetIDFromPlayerState(APlayerState *PlayerState, FBPUniqueNetId &UniqueNetId)
{
	if (!PlayerState)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetUniqueNetIdFromPlayerState received a bad PlayerState!"));
		return;
	}

	UniqueNetId.SetUniqueNetId(PlayerState->GetUniqueId().GetUniqueNetId());
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetUniqueNetIdFromPlayerState couldn't get the player uniquenetid!"));
	}
	return;
}

bool UAdvancedSessionsLibrary::IsValidUniqueNetID(const FBPUniqueNetId &UniqueNetId)
{
	return UniqueNetId.IsValid();
}

bool UAdvancedSessionsLibrary::EqualEqual_UNetIDUnetID(const FBPUniqueNetId &A, const FBPUniqueNetId &B)
{	
	return ((A.IsValid() && B.IsValid()) && (*A.GetUniqueNetId() == *B.GetUniqueNetId()));
}

FUniqueNetIdRepl UAdvancedSessionsLibrary::Conv_BPUniqueIDToUniqueNetIDRepl(const FBPUniqueNetId& InUniqueID)
{
	return FUniqueNetIdRepl(InUniqueID.GetUniqueNetId()->AsShared());
}

void UAdvancedSessionsLibrary::SetPlayerName(APlayerController *PlayerController, FString PlayerName)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("SetLocalPlayerNameFromController Bad Player Controller!"));
		return;
	}

	if (APlayerState* PlayerState = (PlayerController != NULL) ? PlayerController->PlayerState : NULL)
	{
		PlayerState->SetPlayerName(PlayerName);
		return;
	}
	else
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("SetLocalPlayerNameFromController had a bad player state!"));
	}
}

void UAdvancedSessionsLibrary::GetPlayerName(APlayerController *PlayerController, FString &PlayerName)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetLocalPlayerNameFromController Bad Player Controller!"));
		return;
	}

	if (APlayerState* PlayerState = (PlayerController != NULL) ? PlayerController->PlayerState : NULL)
	{
		PlayerName = PlayerState->GetPlayerName();
		return;
	}
	else
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetLocalPlayerNameFromController had a bad player state!"));
	}
}

void UAdvancedSessionsLibrary::GetNumberOfNetworkPlayers(UObject* WorldContextObject, int32 &NumNetPlayers)
{
	//Get World
	UWorld* TheWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (!TheWorld)
	{
		UE_LOG(AdvancedSessionsLog, Warning, TEXT("GetNumberOfNetworkPlayers Failed to get World()!"));
		return;
	}

	NumNetPlayers = TheWorld->GetGameState()->PlayerArray.Num();
}

bool UAdvancedSessionsLibrary::ServerTravel(UObject* WorldContextObject, const FString& FURL, bool bAbsolute, bool bShouldSkipGameNotify)
{
	if (!WorldContextObject)
	{
		return false;
	}

	//using a context object to get the world
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	if (World)
	{
		return World->ServerTravel(FURL, bAbsolute, bShouldSkipGameNotify);
	}

	return false;
}