// Fill out your copyright notice in the Description page of Project Settings.
#include "OnlineSubSystemHeader.h"
#include "AdvancedFriendsLibrary.h"


// This is taken directly from UE4 - OnlineSubsystemSteamPrivatePCH.h as a fix for the array_count macro

// @todo Steam: Steam headers trigger secure-C-runtime warnings in Visual C++. Rather than mess with _CRT_SECURE_NO_WARNINGS, we'll just
//	disable the warnings locally. Remove when this is fixed in the SDK
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

#pragma push_macro("ARRAY_COUNT")
#undef ARRAY_COUNT

#include <steam/steam_api.h>

#pragma pop_macro("ARRAY_COUNT")

#endif

// @todo Steam: See above
#ifdef _MSC_VER
#pragma warning(pop)
#endif

//General Log
DEFINE_LOG_CATEGORY(AdvancedFriendsLog);


int32 UAdvancedFriendsLibrary::GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId)
{

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("IsAFriend Had a bad UniqueNetId!"));
		return 0;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

		return SteamFriends()->GetFriendSteamLevel(id);
	}
#endif

	return 0;


}

bool UAdvancedFriendsLibrary::RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("RequestSteamFriendInfo Had a bad UniqueNetId!"));
		return false;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

		return !SteamFriends()->RequestUserInformation(id, false);
	}
#endif

	UE_LOG(AdvancedFriendsLog, Warning, TEXT("RequestSteamFriendInfo Couldn't init steamAPI!"));
	return false;
}

UTexture2D * UAdvancedFriendsLibrary::GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid())
	{
		UE_LOG(AdvancedFriendsLog, Warning, TEXT("GetSteamFriendAvatar Had a bad UniqueNetId!"));
		Result = EBlueprintAsyncResultSwitch::OnFailure;
		return nullptr;
	}

	uint32 Width = 0;
	uint32 Height = 0;

	if (SteamAPI_Init())
	{
		//Getting the PictureID from the SteamAPI and getting the Size with the ID
		//virtual bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly ) = 0;

		
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());
		int Picture = 0;
		
		switch(AvatarSize)
		{
		case SteamAvatarSize::SteamAvatar_Small: Picture = SteamFriends()->GetSmallFriendAvatar(id); break;
		case SteamAvatarSize::SteamAvatar_Medium: Picture = SteamFriends()->GetMediumFriendAvatar(id); break;
		case SteamAvatarSize::SteamAvatar_Large: Picture = SteamFriends()->GetLargeFriendAvatar(id); break;
		default: break;
		}

		if (Picture == -1)
		{
			Result = EBlueprintAsyncResultSwitch::AsyncLoading;
			return NULL;
		}

		SteamUtils()->GetImageSize(Picture, &Width, &Height);

		// STOLEN FROM ANSWERHUB :p, then fixed because answerhub wasn't releasing the memory O.o

		if (Width > 0 && Height > 0)
		{
			//Creating the buffer "oAvatarRGBA" and then filling it with the RGBA Stream from the Steam Avatar
			uint8 *oAvatarRGBA = new uint8[Width * Height * 4];


			//Filling the buffer with the RGBA Stream from the Steam Avatar and creating a UTextur2D to parse the RGBA Steam in
			SteamUtils()->GetImageRGBA(Picture, (uint8*)oAvatarRGBA, 4 * Height * Width * sizeof(char));


			// Removed as I changed the image bit code to be RGB, I think the original author was unaware that there were different pixel formats
			/*
			//Swap R and B channels because for some reason the games whack
			for (uint32 i = 0; i < (Width * Height * 4); i += 4)
			{
				uint8 Temp = oAvatarRGBA[i + 0];
				oAvatarRGBA[i + 0] = oAvatarRGBA[i + 2];
				oAvatarRGBA[i + 2] = Temp;
			}*/

			UTexture2D* Avatar = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);

			// Switched to a Memcpy instead of byte by byte transer
			uint8* MipData = (uint8*)Avatar->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(MipData, (void*)oAvatarRGBA, Height * Width * 4);
			Avatar->PlatformData->Mips[0].BulkData.Unlock();

			// Original implementation was missing this!!
			// the hell man......
			delete[] oAvatarRGBA;

			//Setting some Parameters for the Texture and finally returning it
			Avatar->PlatformData->NumSlices = 1;
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();

			Result = EBlueprintAsyncResultSwitch::OnSuccess;
			return Avatar;
		}
		else
		{
			UE_LOG(AdvancedFriendsLog, Warning, TEXT("Bad Height / Width with steam avatar!"));
		}

		Result = EBlueprintAsyncResultSwitch::OnFailure;
		return nullptr;
	}
#endif

	UE_LOG(AdvancedFriendsLog, Warning, TEXT("STEAM Couldn't be verified as initialized"));
	Result = EBlueprintAsyncResultSwitch::OnFailure;
	return nullptr;
}

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

	if (SessionInterface->SendSessionInviteToFriends(Player->GetControllerId(), GameSessionName, List))
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

	if (SessionInterface->SendSessionInviteToFriend(Player->GetControllerId(), GameSessionName, *FriendUniqueNetId.GetUniqueNetId()))
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