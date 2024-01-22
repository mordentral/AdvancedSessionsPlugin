// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedIdentityLibrary.h"

//General Log
DEFINE_LOG_CATEGORY(AdvancedIdentityLog);


void UAdvancedIdentityLibrary::GetPlayerAuthToken(UObject* WorldContextObject, APlayerController * PlayerController, FString & AuthToken, EBlueprintResultSwitch &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetPlayerAuthToken was passed a bad player controller!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetPlayerAuthToken failed to get LocalPlayer!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetPlayerAuthToken Failed to get identity interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	AuthToken = IdentityInterface->GetAuthToken(Player->GetControllerId());
	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedIdentityLibrary::GetPlayerNickname(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetID, FString & PlayerNickname)
{ 
	if (!UniqueNetID.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetPlayerNickname was passed a bad player uniquenetid!"));
		return;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetPlayerNickname Failed to get identity interface!"));
		return;
	}
	PlayerNickname = IdentityInterface->GetPlayerNickname(*UniqueNetID.GetUniqueNetId());
}


void UAdvancedIdentityLibrary::GetLoginStatus(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetID, EBPLoginStatus & LoginStatus, EBlueprintResultSwitch &Result)
{
	if (!UniqueNetID.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetLoginStatus was passed a bad player uniquenetid!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetLoginStatus Failed to get identity interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	LoginStatus = (EBPLoginStatus)IdentityInterface->GetLoginStatus(*UniqueNetID.GetUniqueNetId());
	Result = EBlueprintResultSwitch::OnSuccess;
}


void UAdvancedIdentityLibrary::GetAllUserAccounts(UObject* WorldContextObject, TArray<FBPUserOnlineAccount> & AccountInfos, EBlueprintResultSwitch &Result)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetAllUserAccounts Failed to get identity interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	TArray<TSharedPtr<FUserOnlineAccount>> accountInfos = IdentityInterface->GetAllUserAccounts();

	for (int i = 0; i < accountInfos.Num(); ++i)
	{
		AccountInfos.Add(FBPUserOnlineAccount(accountInfos[i]));
	}

	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedIdentityLibrary::GetUserAccount(UObject* WorldContextObject, const FBPUniqueNetId & UniqueNetId, FBPUserOnlineAccount & AccountInfo, EBlueprintResultSwitch &Result)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Online::GetIdentityInterface(World);

	if(!UniqueNetId.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccount was passed a bad unique net id!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (!IdentityInterface.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccount Failed to get identity interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	TSharedPtr<FUserOnlineAccount> accountInfo = IdentityInterface->GetUserAccount(*UniqueNetId.GetUniqueNetId());

	if (!accountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccount Failed to get the account!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	AccountInfo.UserAccountInfo = accountInfo;
	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedIdentityLibrary::GetUserAccountAccessToken(const FBPUserOnlineAccount & AccountInfo, FString & AccessToken)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountAccessToken was passed an invalid account!"));
		return;
	}

	AccessToken = AccountInfo.UserAccountInfo->GetAccessToken();
}

void UAdvancedIdentityLibrary::GetUserAccountAuthAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AuthAttribute, EBlueprintResultSwitch &Result)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountAuthAttribute was passed an invalid account!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (!AccountInfo.UserAccountInfo->GetAuthAttribute(AttributeName, AuthAttribute))
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountAuthAttribute couldn't find the attribute!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedIdentityLibrary::SetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, const FString & NewAttributeValue, EBlueprintResultSwitch &Result)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("SetUserAccountAuthAttribute was passed an invalid account!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (!AccountInfo.UserAccountInfo->SetUserAttribute(AttributeName, NewAttributeValue))
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("SetUserAccountAuthAttribute was unable to set the attribute!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedIdentityLibrary::GetUserID(const FBPUserOnlineAccount & AccountInfo, FBPUniqueNetId & UniqueNetID)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserID was passed an invalid account!"));
		return;
	}

	
	UniqueNetID.SetUniqueNetId(AccountInfo.UserAccountInfo->GetUserId());
}

void UAdvancedIdentityLibrary::GetUserAccountRealName(const FBPUserOnlineAccount & AccountInfo, FString & UserName)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountRealName was passed an invalid account!"));
		return;
	}


	UserName = AccountInfo.UserAccountInfo->GetRealName();
}

void UAdvancedIdentityLibrary::GetUserAccountDisplayName(const FBPUserOnlineAccount & AccountInfo, FString & DisplayName)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountDisplayName was passed an invalid account!"));
		return;
	}


	DisplayName = AccountInfo.UserAccountInfo->GetDisplayName();
}

void UAdvancedIdentityLibrary::GetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AttributeValue, EBlueprintResultSwitch &Result)
{
	if (!AccountInfo.UserAccountInfo.IsValid())
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountAttribute was passed an invalid account!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	if (!AccountInfo.UserAccountInfo->GetUserAttribute(AttributeName, AttributeValue))
	{
		UE_LOG(AdvancedIdentityLog, Warning, TEXT("GetUserAccountAttribute failed to get user attribute!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	Result = EBlueprintResultSwitch::OnSuccess;
}