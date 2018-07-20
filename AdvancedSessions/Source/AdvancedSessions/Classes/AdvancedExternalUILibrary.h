// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "Engine/LocalPlayer.h"
#include "OnlineSubsystem.h"
#include "BlueprintDataDefinitions.h"
//#include "OnlineFriendsInterface.h"
//#include "OnlineUserInterface.h"
//#include "OnlineMessageInterface.h"
//#include "OnlinePresenceInterface.h"
//#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"

//#include "UObjectIterator.h"

#include "AdvancedExternalUILibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedExternalUILog, Log, All);

UCLASS()
class UAdvancedExternalUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* External UI Functions *************//

	// Show the UI that handles the Friends list
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result"))
	static void ShowFriendsUI(APlayerController *PlayerController, EBlueprintResultSwitch &Result);

	// Show the UI that handles inviting people to your game
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result"))
	static void ShowInviteUI(APlayerController *PlayerController, EBlueprintResultSwitch &Result);

	// Show the UI that shows the leaderboard (doesn't work with steam)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result"))
	static void ShowLeaderBoardUI(FString LeaderboardName, EBlueprintResultSwitch &Result);

	// Show the UI that shows a web URL
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result", AutoCreateRefTerm = "AllowedDomains"))
	static void ShowWebURLUI(FString URLToShow, EBlueprintResultSwitch &Result, TArray<FString>& AllowedDomains, bool bEmbedded = false , bool bShowBackground = false, bool bShowCloseButton = false, int32 OffsetX = 0, int32 OffsetY = 0, int32 SizeX = 0, int32 SizeY = 0);

	// Show the UI that shows a web URL
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI")
	static void CloseWebURLUI();


	// Show the UI that shows the profile of a uniquenetid
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result"))
	static void ShowProfileUI(const FBPUniqueNetId PlayerViewingProfile, const FBPUniqueNetId PlayerToViewProfileOf, EBlueprintResultSwitch &Result);

	// Show the UI that shows the account upgrade UI (doesn't work with steam)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedExternalUI", meta = (ExpandEnumAsExecs = "Result"))
	static void ShowAccountUpgradeUI(const FBPUniqueNetId PlayerRequestingAccountUpgradeUI, EBlueprintResultSwitch &Result);

};	
