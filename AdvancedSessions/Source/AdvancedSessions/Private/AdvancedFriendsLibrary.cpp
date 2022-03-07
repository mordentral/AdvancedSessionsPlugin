// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedFriendsLibrary.h"



// This is taken directly from UE4 - OnlineSubsystemSteamPrivatePCH.h as a fix for the array_count macro

//General Log
DEFINE_LOG_CATEGORY(AdvancedFriendsLog);

void UAdvancedFriendsLibrary::SendSessionInviteToFriends(APlayerController *PlayerController, const TArray<FBPUniqueNetId> &Friends, EBlueprintResultSwitch &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (Friends.Num() < 1)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had no friends in invitation array!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Failed to get session interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend failed to get LocalPlayer!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	TArray<TSharedRef<const FUniqueNetId>> List;
	for (int i = 0; i < Friends.Num(); i++)
	{
		TSharedRef<const FUniqueNetId> val(Friends[i].UniqueNetId.ToSharedRef());
		//TSharedRef<const FUniqueNetId> val(Friends[i].GetUniqueNetId());
		List.Add(val);
	}

	if (SessionInterface->SendSessionInviteToFriends(Player->GetControllerId(), NAME_GameSession, List))
	{
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}

	Result = EBlueprintResultSwitch::OnFailure;
	return;
}

void UAdvancedFriendsLibrary::SendSessionInviteToFriend(APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId, EBlueprintResultSwitch &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (!FriendUniqueNetId.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad UniqueNetId!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Failed to get session interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend failed to get LocalPlayer!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (SessionInterface->SendSessionInviteToFriend(Player->GetControllerId(), NAME_GameSession, *FriendUniqueNetId.GetUniqueNetId()))
	{
		Result = EBlueprintResultSwitch::OnSuccess;
		return;
	}

	Result = EBlueprintResultSwitch::OnFailure;
	return;
}

void UAdvancedFriendsLibrary::GetFriend(APlayerController *PlayerController, const FBPUniqueNetId FriendUniqueNetId, FBPFriendInfo &Friend)
{

	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriend Had a bad Player Controller!"));
		return;
	}

	if (!FriendUniqueNetId.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriend Had a bad UniqueNetId!"));
		return;
	}

	IOnlineFriendsPtr FriendsInterface = Online::GetFriendsInterface();

	if (!FriendsInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriend Failed to get friends interface!"));
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriend failed to get LocalPlayer!"));
		return;
	}

	TSharedPtr<FOnlineFriend> fr = FriendsInterface->GetFriend(Player->GetControllerId(), *FriendUniqueNetId.GetUniqueNetId(), EFriendsLists::ToString(EFriendsLists::Default));
	if (fr.IsValid())
	{
		FOnlineUserPresence pres = fr->GetPresence();
		Friend.DisplayName = fr->GetDisplayName();
		Friend.OnlineState = ((EBPOnlinePresenceState)((int32)pres.Status.State));
		Friend.RealName = fr->GetRealName();
		Friend.UniqueNetId.SetUniqueNetId(fr->GetUserId());
		Friend.bIsPlayingSameGame = pres.bIsPlayingThisGame;

		Friend.PresenceInfo.bHasVoiceSupport = pres.bHasVoiceSupport;
		Friend.PresenceInfo.bIsJoinable = pres.bIsJoinable;
		Friend.PresenceInfo.bIsOnline = pres.bIsOnline;
		Friend.PresenceInfo.bIsPlaying = pres.bIsPlaying;
		Friend.PresenceInfo.bIsPlayingThisGame = pres.bIsPlayingThisGame;
		Friend.PresenceInfo.PresenceState = ((EBPOnlinePresenceState)((int32)pres.Status.State));
		Friend.PresenceInfo.StatusString = pres.Status.StatusStr;
	}
}

void UAdvancedFriendsLibrary::IsAFriend(APlayerController *PlayerController, const FBPUniqueNetId UniqueNetId, bool &IsFriend)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("IsAFriend Had a bad Player Controller!"));
		return;
	}

	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("IsAFriend Had a bad UniqueNetId!"));
		return;
	}

	IOnlineFriendsPtr FriendsInterface = Online::GetFriendsInterface();

	if (!FriendsInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("IsAFriend Failed to get friends interface!"));
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("IsAFriend Failed to get LocalPlayer!"));
		return;
	}

	IsFriend = FriendsInterface->IsFriend(Player->GetControllerId(), *UniqueNetId.GetUniqueNetId(), EFriendsLists::ToString(EFriendsLists::Default));
}

void UAdvancedFriendsLibrary::GetStoredRecentPlayersList(FBPUniqueNetId UniqueNetId, TArray<FBPOnlineRecentPlayer> &PlayersList)
{
	IOnlineFriendsPtr FriendsInterface = Online::GetFriendsInterface();
	
	if (!FriendsInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetRecentPlayersList Failed to get friends interface!"));
		return;
	}

	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetRecentPlayersList Failed was given an invalid UniqueNetId!"));
		return;
	}

	TArray< TSharedRef<FOnlineRecentPlayer> > PlayerList;
	
	// For now getting all namespaces
	FriendsInterface->GetRecentPlayers(*(UniqueNetId.GetUniqueNetId()),"", PlayerList);

	for (int32 i = 0; i < PlayerList.Num(); i++)
	{
		TSharedRef<FOnlineRecentPlayer> Player = PlayerList[i];
		FBPOnlineRecentPlayer BPF;
		BPF.DisplayName = Player->GetDisplayName();
		BPF.RealName = Player->GetRealName();
		BPF.UniqueNetId.SetUniqueNetId(Player->GetUserId());
		PlayersList.Add(BPF);
	}
}

void UAdvancedFriendsLibrary::GetStoredFriendsList(APlayerController *PlayerController, TArray<FBPFriendInfo> &FriendsList)
{

	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriendsList Had a bad Player Controller!"));
		return;
	}

	IOnlineFriendsPtr FriendsInterface = Online::GetFriendsInterface();
	
	if (!FriendsInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriendsList Failed to get friends interface!"));
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetFriendsList Failed to get LocalPlayer!"));
		return;
	}


	TArray< TSharedRef<FOnlineFriend> > FriendList;
	FriendsInterface->GetFriendsList(Player->GetControllerId(), EFriendsLists::ToString((EFriendsLists::Default)), FriendList);

	for (int32 i = 0; i < FriendList.Num(); i++)
	{
		TSharedRef<FOnlineFriend> Friend = FriendList[i];
		FBPFriendInfo BPF;
		FOnlineUserPresence pres = Friend->GetPresence();

		BPF.OnlineState = ((EBPOnlinePresenceState)((int32)pres.Status.State));
		BPF.DisplayName = Friend->GetDisplayName();
		BPF.RealName = Friend->GetRealName();
		BPF.UniqueNetId.SetUniqueNetId(Friend->GetUserId());
		BPF.bIsPlayingSameGame = pres.bIsPlayingThisGame;

		BPF.PresenceInfo.bIsOnline = pres.bIsOnline;
		BPF.PresenceInfo.bHasVoiceSupport = pres.bHasVoiceSupport;
		BPF.PresenceInfo.bIsPlaying = pres.bIsPlaying;
		BPF.PresenceInfo.PresenceState = ((EBPOnlinePresenceState)((int32)pres.Status.State));
		BPF.PresenceInfo.StatusString = pres.Status.StatusStr;
		BPF.PresenceInfo.bIsJoinable = pres.bIsJoinable;
		BPF.PresenceInfo.bIsPlayingThisGame = pres.bIsPlayingThisGame;

		FriendsList.Add(BPF);
	}
}