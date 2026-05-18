// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedVoiceLibrary.h"


//General Log
DEFINE_LOG_CATEGORY(AdvancedVoiceLog);

void UAdvancedVoiceLibrary::IsHeadsetPresent(UObject* WorldContextObject, bool & bHasHeadset, uint8 LocalPlayerNum)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		bHasHeadset = false;
		UE_LOGF(AdvancedVoiceLog, Warning, "Check For Headset couldn't get the voice interface!");
		return;
	}

	bHasHeadset = VoiceInterface->IsHeadsetPresent(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::StartNetworkedVoice(UObject* WorldContextObject, uint8 LocalPlayerNum)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Start Networked Voice couldn't get the voice interface!");
		return;
	}

	VoiceInterface->StartNetworkedVoice(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::StopNetworkedVoice(UObject* WorldContextObject, uint8 LocalPlayerNum)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Start Networked Voice couldn't get the voice interface!");
		return;
	}

	VoiceInterface->StopNetworkedVoice(LocalPlayerNum);
}

bool UAdvancedVoiceLibrary::RegisterLocalTalker(UObject* WorldContextObject, uint8 LocalPlayerNum)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Register Local Talker couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->RegisterLocalTalker(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::RegisterAllLocalTalkers(UObject* WorldContextObject)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Register Local Talkers couldn't get the voice interface!");
		return;
	}

	VoiceInterface->RegisterLocalTalkers();
}


void UAdvancedVoiceLibrary::UnRegisterLocalTalker(UObject* WorldContextObject, uint8 LocalPlayerNum)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Unregister Local Talker couldn't get the voice interface!");
		return;
	}

	VoiceInterface->UnregisterLocalTalker(LocalPlayerNum);
}

void UAdvancedVoiceLibrary::UnRegisterAllLocalTalkers(UObject* WorldContextObject)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "UnRegister All Local Talkers couldn't get the voice interface!");
		return;
	}

	VoiceInterface->UnregisterLocalTalkers();
}

bool UAdvancedVoiceLibrary::RegisterRemoteTalker(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Register Remote Talker was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Register Remote Talker couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->RegisterRemoteTalker(*UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::UnRegisterRemoteTalker(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "UnRegister Remote Talker was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "UnRegister Remote Talker couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->UnregisterRemoteTalker(*UniqueNetId.GetUniqueNetId());
}

void UAdvancedVoiceLibrary::RemoveAllRemoteTalkers(UObject* WorldContextObject)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Remove All Remote Talkers couldn't get the voice interface!");
		return;
	}

	VoiceInterface->RemoveAllRemoteTalkers();
}

bool UAdvancedVoiceLibrary::IsLocalPlayerTalking(UObject* WorldContextObject, uint8 LocalPlayerNum)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Is Local Player Talking couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->IsLocalPlayerTalking(LocalPlayerNum);
}

bool UAdvancedVoiceLibrary::IsRemotePlayerTalking(UObject* WorldContextObject, const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Is Remote Player Talking was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Is Remote Player Talking couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->IsRemotePlayerTalking(*UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::IsPlayerMuted(UObject* WorldContextObject, uint8 LocalUserNumChecking, const FBPUniqueNetId& UniqueNetId)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Is Player Muted was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Is Player Muted couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->IsMuted(LocalUserNumChecking, *UniqueNetId.GetUniqueNetId());
}

bool UAdvancedVoiceLibrary::MuteRemoteTalker(UObject* WorldContextObject, uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Mute Remote Talker was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Mute Remote Talker couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->MuteRemoteTalker(LocalUserNum, *UniqueNetId.GetUniqueNetId(), bIsSystemWide);
}

bool UAdvancedVoiceLibrary::UnMuteRemoteTalker(UObject* WorldContextObject, uint8 LocalUserNum, const FBPUniqueNetId& UniqueNetId, bool bIsSystemWide)
{
	if (!UniqueNetId.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Unmute Remote Talker was passed an invalid unique net id!");
		return false;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return false;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		UE_LOGF(AdvancedVoiceLog, Warning, "Unmute Remote Talker couldn't get the voice interface!");
		return false;
	}

	return VoiceInterface->UnmuteRemoteTalker(LocalUserNum, *UniqueNetId.GetUniqueNetId(), bIsSystemWide);
}


void UAdvancedVoiceLibrary::GetNumLocalTalkers(UObject* WorldContextObject, int32 & NumLocalTalkers)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineVoicePtr VoiceInterface = Online::GetVoiceInterface(World);

	if (!VoiceInterface.IsValid())
	{
		NumLocalTalkers = 0;
		UE_LOGF(AdvancedVoiceLog, Warning, "Unmute Remote Talker couldn't get the voice interface!");
		return;
	}

	NumLocalTalkers = VoiceInterface->GetNumLocalTalkers();
}