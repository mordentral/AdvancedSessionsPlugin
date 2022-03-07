// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedVoiceLibrary.h"


//General Log
DEFINE_LOG_CATEGORY(AdvancedVoiceLog);

void UAdvancedVoiceLibrary::IsHeadsetPresent(bool & bHasHeadset, uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		bHasHeadset = false;
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Check For Headset couldn't get the voice interface!"));
		return;
	}

	bHasHeadset = VoiceInterface->IsHeadsetPresent(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::StartNetworkedVoice(uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Start Networked Voice couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->StartNetworkedVoice(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::StopNetworkedVoice(uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Start Networked Voice couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->StopNetworkedVoice(LocalPlayerNum);
}

bool UAdvancedVoiceLibrary::RegisterLocalTalker(uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Register Local Talker couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->RegisterLocalTalker(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::RegisterAllLocalTalkers()
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Register Local Talkers couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->RegisterLocalTalkers();
}


void UAdvancedVoiceLibrary::UnRegisterLocalTalker(uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Unregister Local Talker couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->UnregisterLocalTalker(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::UnRegisterAllLocalTalkers()
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("UnRegister All Local Talkers couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->UnregisterLocalTalkers();
}

bool UAdvancedVoiceLibrary::RegisterRemoteTalker(const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Register Remote Talker was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Register Remote Talker couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->RegisterRemoteTalker(*UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::UnRegisterRemoteTalker(const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("UnRegister Remote Talker was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("UnRegister Remote Talker couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->UnregisterRemoteTalker(*UniqueNetId.GetUniqueNetId());
}

void UAdvancedVoiceLibrary::RemoveAllRemoteTalkers()
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Remove All Remote Talkers couldn't get the voice interface!"));
		return;
	}

	VoiceInterface->RemoveAllRemoteTalkers();
}

bool UAdvancedVoiceLibrary::IsLocalPlayerTalking(uint8 LocalPlayerNum)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Is Local Player Talking couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->IsLocalPlayerTalking(LocalPlayerNum);
}

bool UAdvancedVoiceLibrary::IsRemotePlayerTalking(const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Is Remote Player Talking was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Is Remote Player Talking couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->IsRemotePlayerTalking(*UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::IsPlayerMuted(uint8 LocalUserNumChecking, const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Is Player Muted was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Is Player Muted couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->IsMuted(LocalUserNumChecking, *UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::MuteRemoteTalker(uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Mute Remote Talker was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Mute Remote Talker couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->MuteRemoteTalker(LocalUserNum, *UniqueNetId.GetUniqueNetId(), bIsSystemWide);
}

bool UAdvancedVoiceLibrary::UnMuteRemoteTalker(uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Unmute Remote Talker was passed an invalid unique net id!"));
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Unmute Remote Talker couldn't get the voice interface!"));
		return false;
	}

	return VoiceInterface->UnmuteRemoteTalker(LocalUserNum, *UniqueNetId.GetUniqueNetId(), bIsSystemWide);
}


void UAdvancedVoiceLibrary::GetNumLocalTalkers(int32 & NumLocalTalkers)
{
	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface();

	if (!VoiceInterface.IsValid())
	{
		NumLocalTalkers = 0;
		UE_LOG(AdvancedVoiceLog, Warning, TEXT("Unmute Remote Talker couldn't get the voice interface!"));
		return;
	}

	NumLocalTalkers = VoiceInterface->GetNumLocalTalkers();
}