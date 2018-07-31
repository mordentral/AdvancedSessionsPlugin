// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

#include "UObject/UObjectIterator.h"

#include "AdvancedIdentityLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedIdentityLog, Log, All);

UCLASS()
class UAdvancedIdentityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//********* Identity Functions *************//

	// Get the login status of a local player
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity", meta = (ExpandEnumAsExecs = "Result"))
	static void GetLoginStatus(const FBPUniqueNetId & UniqueNetID, EBPLoginStatus & LoginStatus, EBlueprintResultSwitch &Result);

	// Get the auth token for a local player
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity", meta = (ExpandEnumAsExecs = "Result"))
	static void GetPlayerAuthToken(APlayerController * PlayerController, FString & AuthToken, EBlueprintResultSwitch &Result);

	// Get a players nickname
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity")
	static void GetPlayerNickname(const FBPUniqueNetId & UniqueNetID, FString & PlayerNickname);

	//********* User Account Functions *************//

	// Get a users account
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetUserAccount(const FBPUniqueNetId & UniqueNetId, FBPUserOnlineAccount & AccountInfo, EBlueprintResultSwitch &Result);

	// Get all known users accounts
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetAllUserAccounts(TArray<FBPUserOnlineAccount> & AccountInfos, EBlueprintResultSwitch &Result);

	// Get a user account access token
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountAccessToken(const FBPUserOnlineAccount & AccountInfo, FString & AccessToken);

	// Get a user account Auth attribute (depends on subsystem)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetUserAccountAuthAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AuthAttribute, EBlueprintResultSwitch &Result);

	// Set a user account attribute (depends on subsystem)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void SetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, const FString & NewAttributeValue, EBlueprintResultSwitch &Result);

	// Get user ID
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserID(const FBPUserOnlineAccount & AccountInfo, FBPUniqueNetId & UniqueNetID);

	// Get user accounts real name if possible
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountRealName(const FBPUserOnlineAccount & AccountInfo, FString & UserName);

	// Get user account display name if possible
	UFUNCTION(BlueprintPure, Category = "Online|AdvancedIdentity|UserAccount")
	static void GetUserAccountDisplayName(const FBPUserOnlineAccount & AccountInfo, FString & DisplayName);

	// Get user account attribute (depends on subsystem)
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedIdentity|UserAccount", meta = (ExpandEnumAsExecs = "Result"))
	static void GetUserAccountAttribute(const FBPUserOnlineAccount & AccountInfo, const FString & AttributeName, FString & AttributeValue, EBlueprintResultSwitch &Result);


};	
