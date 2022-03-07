// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedFriendsGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

//General Log
DEFINE_LOG_CATEGORY(AdvancedFriendsInterfaceLog);

UAdvancedFriendsGameInstance::UAdvancedFriendsGameInstance(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
	, bCallFriendInterfaceEventsOnPlayerControllers(true)
	, bCallIdentityInterfaceEventsOnPlayerControllers(true)
	, bCallVoiceInterfaceEventsOnPlayerControllers(true)
	, bEnableTalkingStatusDelegate(true)
	, SessionInviteReceivedDelegate(FOnSessionInviteReceivedDelegate::CreateUObject(this, &ThisClass::OnSessionInviteReceivedMaster))
	, SessionInviteAcceptedDelegate(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionInviteAcceptedMaster))
	, PlayerTalkingStateChangedDelegate(FOnPlayerTalkingStateChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerTalkingStateChangedMaster))
	, PlayerLoginChangedDelegate(FOnLoginChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerLoginChangedMaster))
	, PlayerLoginStatusChangedDelegate(FOnLoginStatusChangedDelegate::CreateUObject(this, &ThisClass::OnPlayerLoginStatusChangedMaster))
{
}

void UAdvancedFriendsGameInstance::Shutdown()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());
	
	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsGameInstance Failed to get session system!"));
		//return;
	}
	else
	{
		// Clear all of the delegate handles here
		SessionInterface->ClearOnSessionUserInviteAcceptedDelegate_Handle(SessionInviteAcceptedDelegateHandle);
		SessionInterface->ClearOnSessionInviteReceivedDelegate_Handle(SessionInviteReceivedDelegateHandle);
	}


	if (bEnableTalkingStatusDelegate)
	{
		IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(GetWorld());

		if (VoiceInterface.IsValid())
		{
			VoiceInterface->ClearOnPlayerTalkingStateChangedDelegate_Handle(PlayerTalkingStateChangedDelegateHandle);
		}
		else
		{

			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get voice interface!"));
		}
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(GetWorld());

	if (IdentityInterface.IsValid())
	{
		IdentityInterface->ClearOnLoginChangedDelegate_Handle(PlayerLoginChangedDelegateHandle);
		

		// I am just defaulting to player 1
		IdentityInterface->ClearOnLoginStatusChangedDelegate_Handle(0, PlayerLoginStatusChangedDelegateHandle);
	}


	Super::Shutdown();
}

void UAdvancedFriendsGameInstance::Init()
{
	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(GetWorld());//OnlineSub->GetSessionInterface();

	if (SessionInterface.IsValid())
	{
		// Currently doesn't store a handle or assign a delegate to any local player beyond the first.....should handle?
		// Thought about directly handling it but friends for multiple players probably isn't required
		// Iterating through the local player TArray only works if it has had players assigned to it, most of the online interfaces don't support
		// Multiple logins either (IE: Steam)
		SessionInviteAcceptedDelegateHandle = SessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(SessionInviteAcceptedDelegate);

		SessionInviteReceivedDelegateHandle = SessionInterface->AddOnSessionInviteReceivedDelegate_Handle(SessionInviteReceivedDelegate);
	}
	else
	{
		UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get session interface!"));
		//return;
	}

	// Beginning work on the voice interface
	if (bEnableTalkingStatusDelegate)
	{
		IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(GetWorld());

		if (VoiceInterface.IsValid())
		{
			PlayerTalkingStateChangedDelegateHandle = VoiceInterface->AddOnPlayerTalkingStateChangedDelegate_Handle(PlayerTalkingStateChangedDelegate);
		}
		else
		{

			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get voice interface!"));
		}
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(GetWorld());

	if (IdentityInterface.IsValid())
	{
		PlayerLoginChangedDelegateHandle = IdentityInterface->AddOnLoginChangedDelegate_Handle(PlayerLoginChangedDelegate);

		// Just defaulting to player 1
		PlayerLoginStatusChangedDelegateHandle = IdentityInterface->AddOnLoginStatusChangedDelegate_Handle(0, PlayerLoginStatusChangedDelegate);
	}
	else
	{
		UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get identity interface!"));
	}


	Super::Init();
}

/*void UAdvancedFriendsGameInstance::PostLoad()
{
	Super::PostLoad();
}*/


// Removed because it never gets called by the online subsystems
/*void UAdvancedFriendsGameInstance::OnSessionInviteReceivedMaster(const FUniqueNetId &InvitedPlayer, const FUniqueNetId &FriendInviting, const FOnlineSessionSearchResult& Session)
{
	// Just call the blueprint event to let the user handle this

	FBPUniqueNetId IP, FI;

	IP.SetUniqueNetId(&InvitedPlayer);

	FI.SetUniqueNetId(&FriendInviting);

	FBlueprintSessionResult BPS;
	BPS.OnlineResult = Session;
	OnSessionInviteReceived(IP,FI,BPS);

	TArray<class APlayerState*>& PlayerArray = GetWorld()->GetGameState()->PlayerArray;
	const TArray<class ULocalPlayer*>&ControllerArray = this->GetLocalPlayers();

	for (int i = 0; i < ControllerArray.Num(); i++)
	{
		if (*PlayerArray[ControllerArray[i]->PlayerController->NetPlayerIndex]->UniqueId.GetUniqueNetId().Get() == InvitedPlayer)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (ControllerArray[i]->PlayerController->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
			{
				IAdvancedFriendsInterface::Execute_OnSessionInviteReceived(ControllerArray[i]->PlayerController, FI, BPS);
			}
			break;
		}
	}
}*/

void UAdvancedFriendsGameInstance::OnPlayerLoginStatusChangedMaster(int32 PlayerNum, ELoginStatus::Type PreviousStatus, ELoginStatus::Type NewStatus, const FUniqueNetId & NewPlayerUniqueNetID)
{
	EBPLoginStatus OrigStatus = (EBPLoginStatus)PreviousStatus;
	EBPLoginStatus CurrentStatus = (EBPLoginStatus)NewStatus;
	FBPUniqueNetId PlayerID;
	PlayerID.SetUniqueNetId(&NewPlayerUniqueNetID);

	OnPlayerLoginStatusChanged(PlayerNum, OrigStatus,CurrentStatus,PlayerID);


	if (bCallIdentityInterfaceEventsOnPlayerControllers)
	{
		APlayerController* Player = UGameplayStatics::GetPlayerController(GetWorld(), PlayerNum);

		if (Player != NULL)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (Player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
			{
				IAdvancedFriendsInterface::Execute_OnPlayerLoginStatusChanged(Player, OrigStatus, CurrentStatus, PlayerID);
			}
		}
		else
		{
			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get a controller with the specified index in OnPlayerLoginStatusChangedMaster!"));
		}
	}
}

void UAdvancedFriendsGameInstance::OnPlayerLoginChangedMaster(int32 PlayerNum)
{
	OnPlayerLoginChanged(PlayerNum);

	if (bCallIdentityInterfaceEventsOnPlayerControllers)
	{
		APlayerController* Player = UGameplayStatics::GetPlayerController(GetWorld(), PlayerNum);

		if (Player != NULL)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (Player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
			{
				IAdvancedFriendsInterface::Execute_OnPlayerLoginChanged(Player, PlayerNum);
			}
		}
		else
		{
			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get a controller with the specified index in OnPlayerLoginChanged!"));
		}
	}
}

void UAdvancedFriendsGameInstance::OnPlayerTalkingStateChangedMaster(TSharedRef<const FUniqueNetId> PlayerId, bool bIsTalking)
{
	FBPUniqueNetId PlayerTalking;
	PlayerTalking.SetUniqueNetId(PlayerId);
	OnPlayerTalkingStateChanged(PlayerTalking, bIsTalking);

	if (bCallVoiceInterfaceEventsOnPlayerControllers)
	{
		APlayerController* Player = NULL;

		for (const ULocalPlayer* LPlayer : LocalPlayers)
		{
			Player = UGameplayStatics::GetPlayerController(GetWorld(), LPlayer->GetControllerId());

			if (Player != NULL)
			{
				//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
				if (Player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
				{
					IAdvancedFriendsInterface::Execute_OnPlayerVoiceStateChanged(Player, PlayerTalking, bIsTalking);
				}
			}
			else
			{
				UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get a controller with the specified index in OnVoiceStateChanged!"));
			}
		}
	}
}

void UAdvancedFriendsGameInstance::OnSessionInviteReceivedMaster(const FUniqueNetId & PersonInvited, const FUniqueNetId & PersonInviting, const FString& AppId, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (SessionToJoin.IsValid())
	{
		FBlueprintSessionResult BluePrintResult;
		BluePrintResult.OnlineResult = SessionToJoin;

		FBPUniqueNetId PInvited;
		PInvited.SetUniqueNetId(&PersonInvited);

		FBPUniqueNetId PInviting;
		PInviting.SetUniqueNetId(&PersonInviting);


		TArray<APlayerController*> PlayerList;
		GEngine->GetAllLocalPlayerControllers(PlayerList);

		APlayerController* Player = NULL;

		int32 LocalPlayer = 0;
		for (int i = 0; i < PlayerList.Num(); i++)
		{
			if (*PlayerList[i]->PlayerState->GetUniqueId().GetUniqueNetId() == PersonInvited)
			{
				LocalPlayer = i;
				Player = PlayerList[i];
				break;
			}
		}

		OnSessionInviteReceived(LocalPlayer, PInviting, AppId, BluePrintResult);

		//IAdvancedFriendsInterface* TheInterface = NULL;

		if (Player != NULL)
		{
			//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
			if (Player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
			{
				IAdvancedFriendsInterface::Execute_OnSessionInviteReceived(Player, PInviting, BluePrintResult);
			}
		}
		else
		{
			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get a controller with the specified index in OnSessionInviteReceived!"));
		}
	}
	else
	{
		UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Return a bad search result in OnSessionInviteReceived!"));
	}
}

void UAdvancedFriendsGameInstance::OnSessionInviteAcceptedMaster(const bool bWasSuccessful, int32 LocalPlayer, TSharedPtr<const FUniqueNetId> PersonInvited, const FOnlineSessionSearchResult& SessionToJoin)
{
	if (bWasSuccessful)
	{
		if (SessionToJoin.IsValid())
		{

			FBlueprintSessionResult BluePrintResult;
			BluePrintResult.OnlineResult = SessionToJoin;

			FBPUniqueNetId PInvited;
			PInvited.SetUniqueNetId(PersonInvited);

			OnSessionInviteAccepted(LocalPlayer,PInvited, BluePrintResult);

			APlayerController* Player = UGameplayStatics::GetPlayerController(GetWorld(), LocalPlayer);

			//IAdvancedFriendsInterface* TheInterface = NULL;

			if (Player != NULL)
			{
				//Run the Event specific to the actor, if the actor has the interface, otherwise ignore
				if (Player->GetClass()->ImplementsInterface(UAdvancedFriendsInterface::StaticClass()))
				{
					IAdvancedFriendsInterface::Execute_OnSessionInviteAccepted(Player,PInvited, BluePrintResult);
				}
			}
			else
			{ 
				UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Failed to get a controller with the specified index in OnSessionInviteAccepted!"));
			}
		}
		else
		{
			UE_LOG(AdvancedFriendsInterfaceLog, Warning, TEXT("UAdvancedFriendsInstance Return a bad search result in OnSessionInviteAccepted!"));
		}
	}
}