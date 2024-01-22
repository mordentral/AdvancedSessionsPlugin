// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedExternalUILibrary.h"
#include "Engine/LocalPlayer.h"


//General Log
DEFINE_LOG_CATEGORY(AdvancedExternalUILog);

void UAdvancedExternalUILibrary::ShowAccountUpgradeUI(UObject* WorldContextObject, const FBPUniqueNetId PlayerRequestingAccountUpgradeUI, EBlueprintResultSwitch &Result)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowAccountUpgradeUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ExternalUIInterface->ShowAccountUpgradeUI(*PlayerRequestingAccountUpgradeUI.GetUniqueNetId());
	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedExternalUILibrary::ShowProfileUI(UObject* WorldContextObject, const FBPUniqueNetId PlayerViewingProfile, const FBPUniqueNetId PlayerToViewProfileOf, EBlueprintResultSwitch &Result)
{

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowProfileUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}
	
	ExternalUIInterface->ShowProfileUI(*PlayerViewingProfile.GetUniqueNetId(), *PlayerToViewProfileOf.GetUniqueNetId(), NULL);
	Result = EBlueprintResultSwitch::OnSuccess;
}



void UAdvancedExternalUILibrary::ShowWebURLUI(UObject* WorldContextObject, FString URLToShow, EBlueprintResultSwitch &Result, TArray<FString>& AllowedDomains, bool bEmbedded, bool bShowBackground, bool bShowCloseButton, int32 OffsetX, int32 OffsetY, int32 SizeX, int32 SizeY)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowWebURLUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	URLToShow = URLToShow.Replace(TEXT("http://"), TEXT(""));
	URLToShow = URLToShow.Replace(TEXT("https://"), TEXT(""));

	FShowWebUrlParams Params;
	Params.AllowedDomains = AllowedDomains;
	Params.bEmbedded = bEmbedded;
	Params.bShowBackground = bShowBackground;
	Params.bShowCloseButton = bShowCloseButton;
	Params.OffsetX = OffsetX;
	Params.OffsetY = OffsetY;
	Params.SizeX = SizeX;
	Params.SizeY = SizeY;

	ExternalUIInterface->ShowWebURL(URLToShow, Params);
	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedExternalUILibrary::CloseWebURLUI(UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("CloseWebURLUI Failed to get External UI interface!"));
		return;
	}

	ExternalUIInterface->CloseWebURL();
}

void UAdvancedExternalUILibrary::ShowLeaderBoardUI(UObject* WorldContextObject, FString LeaderboardName, EBlueprintResultSwitch &Result)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowLeaderboardsUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ExternalUIInterface->ShowLeaderboardUI(LeaderboardName);
	Result = EBlueprintResultSwitch::OnSuccess;

}


void UAdvancedExternalUILibrary::ShowInviteUI(UObject* WorldContextObject, APlayerController *PlayerController, EBlueprintResultSwitch &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowInviteUI Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowInviteUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowInviteUI Failed to get ULocalPlayer for the given PlayerController!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ExternalUIInterface->ShowInviteUI(Player->GetControllerId(), NAME_GameSession);
	Result = EBlueprintResultSwitch::OnSuccess;
}

void UAdvancedExternalUILibrary::ShowFriendsUI(UObject* WorldContextObject, APlayerController *PlayerController, EBlueprintResultSwitch &Result)
{
	if (!PlayerController)
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowFriendsUI Had a bad Player Controller!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World))
	{
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	IOnlineExternalUIPtr ExternalUIInterface = Online::GetExternalUIInterface(World);

	if (!ExternalUIInterface.IsValid())
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowFriendsUI Failed to get External UI interface!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ULocalPlayer* Player = Cast<ULocalPlayer>(PlayerController->Player);

	if (!Player)
	{
		UE_LOG(AdvancedExternalUILog, Warning, TEXT("ShowFriendsUI Failed to get ULocalPlayer for the given PlayerController!"));
		Result = EBlueprintResultSwitch::OnFailure;
		return;
	}

	ExternalUIInterface->ShowFriendsUI(Player->GetControllerId());
	Result = EBlueprintResultSwitch::OnSuccess;
}