// Fill out your copyright notice in the Description page of Project Settings.
#include "OnlineSubSystemHeader.h"
#include "AdvancedFriendsLibrary.h"

//General Log
DEFINE_LOG_CATEGORY(AdvancedFriendsLog);

void UAdvancedFriendsLibrary::SendSessionInviteToFriends(APlayerController *PlayerController, const TArray<FBPUniqueNetId> &Friends, TEnumAsByte<EBlueprintResultSwitch::Type> &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	if (Friends.Num() < 1)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had no friends in invitation array!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Failed to get session interface!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend failed to get LocalPlayer!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	TArray<TSharedRef<const FUniqueNetId>> List;
	for (int i = 0; i < Friends.Num(); i++)
	{
		TSharedRef<const FUniqueNetId> val(Friends[i].UniqueNetId.ToSharedRef());
		//TSharedRef<const FUniqueNetId> val(Friends[i].GetUniqueNetId());
		List.Add(val);
	}

	if (SessionInterface->SendSessionInviteToFriends(Player->GetControllerId(), GameSessionName, List))
	{
		Result = EBlueprintResultSwitch::Type::OnSuccess;
		return;
	}

	Result = EBlueprintResultSwitch::Type::OnFailure;
	return;
}

void UAdvancedFriendsLibrary::SendSessionInviteToFriend(APlayerController *PlayerController, const FBPUniqueNetId &FriendUniqueNetId, TEnumAsByte<EBlueprintResultSwitch::Type> &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	if (!FriendUniqueNetId.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Had a bad UniqueNetId!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	IOnlineSessionPtr SessionInterface = Online::GetSessionInterface();

	if (!SessionInterface.IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend Failed to get session interface!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("SendSessionInviteToFriend failed to get LocalPlayer!"));
		Result = EBlueprintResultSwitch::Type::OnFailure;
		return;
	}

	if (SessionInterface->SendSessionInviteToFriend(Player->GetControllerId(), GameSessionName, *FriendUniqueNetId.GetUniqueNetId()))
	{
		Result = EBlueprintResultSwitch::Type::OnSuccess;
		return;
	}

	Result = EBlueprintResultSwitch::Type::OnFailure;
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

	TSharedPtr<FOnlineFriend> fr = FriendsInterface->GetFriend(Player->GetControllerId(), *FriendUniqueNetId.GetUniqueNetId(), EFriendsLists::ToString(EFriendsLists::Type::Default));
	if (fr.IsValid())
	{
		Friend.DisplayName = fr->GetDisplayName();
		Friend.OnlineState = ((EBPOnlinePresenceState::Type)((int32)fr->GetPresence().Status.State));
		Friend.RealName = fr->GetRealName();
		Friend.UniqueNetId.SetUniqueNetId(fr->GetUserId());
		Friend.bIsPlayingSameGame = fr->GetPresence().bIsPlayingThisGame;
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

	IsFriend = FriendsInterface->IsFriend(Player->GetControllerId(), *UniqueNetId.GetUniqueNetId(), EFriendsLists::ToString(EFriendsLists::Type::Default));
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
	FriendsInterface->GetFriendsList(Player->GetControllerId(), EFriendsLists::ToString((EFriendsLists::Type::Default)), FriendList);

	for (int32 i = 0; i < FriendList.Num(); i++)
	{
		TSharedRef<FOnlineFriend> Friend = FriendList[i];
		FBPFriendInfo BPF;

		BPF.OnlineState = ((EBPOnlinePresenceState::Type)((int32)Friend->GetPresence().Status.State));
		BPF.DisplayName = Friend->GetDisplayName();
		BPF.RealName = Friend->GetRealName();	
		BPF.UniqueNetId.SetUniqueNetId(Friend->GetUserId());
		FriendsList.Add(BPF);
	}
}