// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedSteamFriendsLibrary.h"
#include "OnlineSubSystemHeader.h"

//General Log
DEFINE_LOG_CATEGORY(AdvancedSteamFriendsLog);


// Clan functions, add in soon
/*int32 UAdvancedSteamFriendsLibrary::GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId)
{

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("IsAFriend Had a bad UniqueNetId!"));
		return 0;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());


		// clan (group) iteration and access functions
		//virtual int GetClanCount() = 0;
		//virtual CSteamID GetClanByIndex(int iClan) = 0;
		//virtual const char *GetClanName(CSteamID steamIDClan) = 0;
		//virtual const char *GetClanTag(CSteamID steamIDClan) = 0;
		// returns the most recent information we have about what's happening in a clan
		//virtual bool GetClanActivityCounts(CSteamID steamIDClan, int *pnOnline, int *pnInGame, int *pnChatting) = 0;
		// for clans a user is a member of, they will have reasonably up-to-date information, but for others you'll have to download the info to have the latest
		//virtual SteamAPICall_t DownloadClanActivityCounts(ARRAY_COUNT(cClansToRequest) CSteamID *psteamIDClans, int cClansToRequest) = 0;

		// requests information about a clan officer list
		// when complete, data is returned in ClanOfficerListResponse_t call result
		// this makes available the calls below
		// you can only ask about clans that a user is a member of
		// note that this won't download avatars automatically; if you get an officer,
		// and no avatar image is available, call RequestUserInformation( steamID, false ) to download the avatar
		//virtual SteamAPICall_t RequestClanOfficerList(CSteamID steamIDClan) = 0;


		// returns the steamID of the clan owner
		//virtual CSteamID GetClanOwner(CSteamID steamIDClan) = 0;
		// returns the number of officers in a clan (including the owner)
		//virtual int GetClanOfficerCount(CSteamID steamIDClan) = 0;
		// returns the steamID of a clan officer, by index, of range [0,GetClanOfficerCount)
		//virtual CSteamID GetClanOfficerByIndex(CSteamID steamIDClan, int iOfficer) = 0;


		return SteamFriends()->GetFriendSteamLevel(id);
	}
#endif

	return 0;
}*/

void UAdvancedSteamFriendsLibrary::GetSteamGroups(TArray<FBPSteamGroupInfo> & SteamGroups)
{
	
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		int numClans = SteamFriends()->GetClanCount();

		for (int i = 0; i < numClans; i++)
		{
			CSteamID SteamGroupID = SteamFriends()->GetClanByIndex(i);

			if(!SteamGroupID.IsValid())
				continue;

			FBPSteamGroupInfo GroupInfo;

			TSharedPtr<const FUniqueNetId> ValueID(new const FUniqueNetIdSteam2(SteamGroupID));
			GroupInfo.GroupID.SetUniqueNetId(ValueID);
			SteamFriends()->GetClanActivityCounts(SteamGroupID, &GroupInfo.numOnline, &GroupInfo.numInGame, &GroupInfo.numChatting);
			GroupInfo.GroupName = FString(UTF8_TO_TCHAR(SteamFriends()->GetClanName(SteamGroupID)));
			GroupInfo.GroupTag = FString(UTF8_TO_TCHAR(SteamFriends()->GetClanTag(SteamGroupID)));

			SteamGroups.Add(GroupInfo);
		}
	}
#endif

}

void UAdvancedSteamFriendsLibrary::GetSteamFriendGamePlayed(const FBPUniqueNetId UniqueNetId, EBlueprintResultSwitch &Result/*, FString & GameName*/, int32 & AppID)
{

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("GetSteamFriendGamePlayed Had a bad UniqueNetId!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

		FriendGameInfo_t GameInfo;
		bool bIsInGame = SteamFriends()->GetFriendGamePlayed(id, &GameInfo);

		if (bIsInGame && GameInfo.m_gameID.IsValid())
		{
			AppID = GameInfo.m_gameID.AppID();

			// Forgot this test and left it in, it is incorrect, you would need restricted access
			// And it would only find games in the local library anyway
			/*char NameBuffer[512];
			int Len = SteamAppList()->GetAppName(GameInfo.m_gameID.AppID(), NameBuffer, 512);

			if (Len != -1) // Invalid
			{
				GameName = FString(UTF8_TO_TCHAR(NameBuffer));
			}*/

			Result = EBlueprintResultSwitch::OnSuccess;
			return;
		}

	}
#endif

	Result = EBlueprintResultSwitch::OnFailure;
}

int32 UAdvancedSteamFriendsLibrary::GetFriendSteamLevel(const FBPUniqueNetId UniqueNetId)
{

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("IsAFriend Had a bad UniqueNetId!"));
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

FString UAdvancedSteamFriendsLibrary::GetSteamPersonaName(const FBPUniqueNetId UniqueNetId)
{

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("GetSteamPersonaName Had a bad UniqueNetId!"));
		return FString(TEXT(""));
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());
		const char* PersonaName = SteamFriends()->GetFriendPersonaName(id);
		return FString(UTF8_TO_TCHAR(PersonaName));
	}
#endif

	return FString(TEXT(""));
}

FBPUniqueNetId UAdvancedSteamFriendsLibrary::CreateSteamIDFromString(const FString SteamID64)
{
	FBPUniqueNetId netId;
	
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!(SteamID64.Len() > 0))
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("CreateSteamIDFromString Had a bad UniqueNetId!"));
		return netId;
	}

	if (SteamAPI_Init())
	{
		// Already does the conversion
		TSharedPtr<const FUniqueNetId> ValueID(new const FUniqueNetIdSteam2(SteamID64));
		//FCString::Atoi64(*SteamID64));
		
		netId.SetUniqueNetId(ValueID);
		return netId;
	}
#endif

	return netId;
}

FBPUniqueNetId UAdvancedSteamFriendsLibrary::GetLocalSteamIDFromSteam()
{
	FBPUniqueNetId netId;

#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		TSharedPtr<const FUniqueNetId> SteamID(new const FUniqueNetIdSteam2(SteamUser()->GetSteamID()));
		netId.SetUniqueNetId(SteamID);
	}
#endif

	return netId;
}

bool UAdvancedSteamFriendsLibrary::RequestSteamFriendInfo(const FBPUniqueNetId UniqueNetId, bool bRequireNameOnly)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("RequestSteamFriendInfo Had a bad UniqueNetId!"));
		return false;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());

		return !SteamFriends()->RequestUserInformation(id, bRequireNameOnly);
	}
#endif

	UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("RequestSteamFriendInfo Couldn't init steamAPI!"));
	return false;
}


bool UAdvancedSteamFriendsLibrary::OpenSteamUserOverlay(const FBPUniqueNetId UniqueNetId, ESteamUserOverlayType DialogType)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("OpenSteamUserOverlay Had a bad UniqueNetId!"));
		return false;
	}

	if (SteamAPI_Init())
	{
		uint64 id = *((uint64*)UniqueNetId.UniqueNetId->GetBytes());
		FString DialogName = EnumToString("ESteamUserOverlayType", (uint8)DialogType);
		SteamFriends()->ActivateGameOverlayToUser(TCHAR_TO_ANSI(*DialogName), id);
		return true;
	}
#endif

	UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("OpenSteamUserOverlay Couldn't init steamAPI!"));
	return false;
}

bool UAdvancedSteamFriendsLibrary::IsOverlayEnabled()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (SteamAPI_Init())
	{
		return SteamUtils()->IsOverlayEnabled();
	}
#endif

	UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("OpenSteamUserOverlay Couldn't init steamAPI!"));
	return false;
}

UTexture2D * UAdvancedSteamFriendsLibrary::GetSteamFriendAvatar(const FBPUniqueNetId UniqueNetId, EBlueprintAsyncResultSwitch &Result, SteamAvatarSize AvatarSize)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
	if (!UniqueNetId.IsValid() || !UniqueNetId.UniqueNetId->IsValid() || UniqueNetId.UniqueNetId->GetType() != STEAM_SUBSYSTEM)
	{
		UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("GetSteamFriendAvatar Had a bad UniqueNetId!"));
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
		// Also fixed image pixel format and switched to a memcpy instead of manual iteration.
		// At some point I should probably reply to that answerhub post with these fixes to prevent people killing their games.....

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
			Avatar->PlatformData->SetNumSlices(1);
			Avatar->NeverStream = true;
			//Avatar->CompressionSettings = TC_EditorIcon;

			Avatar->UpdateResource();

			Result = EBlueprintAsyncResultSwitch::OnSuccess;
			return Avatar;
		}
		else
		{
			UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("Bad Height / Width with steam avatar!"));
		}

		Result = EBlueprintAsyncResultSwitch::OnFailure;
		return nullptr;
	}
#endif

	UE_LOG(AdvancedSteamFriendsLog, Warning, TEXT("STEAM Couldn't be verified as initialized"));
	Result = EBlueprintAsyncResultSwitch::OnFailure;
	return nullptr;
}

bool UAdvancedSteamFriendsLibrary::InitTextFiltering()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		return SteamUtils()->InitFilterText();
	}

#endif

	return false;
}

bool UAdvancedSteamFriendsLibrary::FilterText(FString TextToFilter, EBPTextFilteringContext Context, const FBPUniqueNetId TextSourceID, FString& FilteredText)
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		uint32 BufferLen = TextToFilter.Len() + 10; // Docs say 1 byte excess min, going with 10
		char* OutText = new char[BufferLen];
		
		uint64 id = 0;

		if (TextSourceID.IsValid())
		{
			id = *((uint64*)TextSourceID.UniqueNetId->GetBytes());
		}
		
		// MAC is bugged with current steam version according to epic, they forced it to be the old steam ver
#if PLATFORM_MAC
			// Filters the provided input message and places the filtered result into pchOutFilteredText.
			//   pchOutFilteredText is where the output will be placed, even if no filtering or censoring is performed
			//   nByteSizeOutFilteredText is the size (in bytes) of pchOutFilteredText
			//   pchInputText is the input string that should be filtered, which can be ASCII or UTF-8
			//   bLegalOnly should be false if you want profanity and legally required filtering (where required) and true if you want legally required filtering only
			//   Returns the number of characters (not bytes) filtered.
			int FilterCount = SteamUtils()->FilterText(OutText, BufferLen, TCHAR_TO_ANSI(*TextToFilter), Context == EBPTextFilteringContext::FContext_GameContent);
#else
		int FilterCount = SteamUtils()->FilterText((ETextFilteringContext)Context, id, TCHAR_TO_ANSI(*TextToFilter), OutText, BufferLen);
#endif

		if (FilterCount > 0)
		{
			FilteredText = FString(UTF8_TO_TCHAR(OutText));
			delete[] OutText;
			return true;
		}

		delete[] OutText;
	}

#endif

	FilteredText = TextToFilter;
	return false;
}

bool UAdvancedSteamFriendsLibrary::IsSteamInBigPictureMode()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX

	if (SteamAPI_Init())
	{
		return SteamUtils()->IsSteamInBigPictureMode();
	}

#endif

	return false;
}