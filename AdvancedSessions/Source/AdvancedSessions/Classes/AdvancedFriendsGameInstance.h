// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "BlueprintDataDefinitions.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "Interfaces/OnlineMessageInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UObject/UObjectIterator.h"
#include "AdvancedFriendsInterface.h"

#include "AdvancedFriendsGameInstance.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedFriendsInterfaceLog, Log, All);

UCLASS()
class ADVANCEDSESSIONS_API UAdvancedFriendsGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UAdvancedFriendsGameInstance(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallFriendInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallIdentityInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedFriendsInterface)
	bool bCallVoiceInterfaceEventsOnPlayerControllers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdvancedVoiceInterface)
	bool bEnableTalkingStatusDelegate;

	//virtual void PostLoad() override;
	virtual void Shutdown() override;
	virtual void Init() override;

	//*** Session invite received by local ***//
	FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	// custom handle to join directly from steam ui "Join Game"
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	// custom Steam UI Join User function #Self invite#
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	// custom Steam UI function to client travel #Self invite#
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//const FUniqueNetId& /*UserId*/, const FUniqueNetId& /*FromId*/, const FString& /*AppId*/, const FOnlineSessionSearchResult& /*InviteResult*/
	void OnSessionInviteReceivedMaster(const FUniqueNetId & PersonInvited, const FUniqueNetId & PersonInviting, const FString & AppId, const FOnlineSessionSearchResult& SessionToJoin);

	// After a session invite has been accepted by the local player this event is triggered, call JoinSession on the session result to join it
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteReceived(int32 LocalPlayerNum, FBPUniqueNetId PersonInviting, const FString& AppId, const FBlueprintSessionResult& SessionToJoin);

	//*** Session invite accepted by local ***//
	FOnSessionUserInviteAcceptedDelegate SessionInviteAcceptedDelegate;
	FDelegateHandle SessionInviteAcceptedDelegateHandle;

	void OnSessionInviteAcceptedMaster(const bool bWasSuccessful, int32 LocalPlayer, TSharedPtr<const FUniqueNetId> PersonInviting, const FOnlineSessionSearchResult& SessionToJoin);

	// After a session invite has been accepted by the local player this event is triggered, call JoinSession on the session result to join it
	// This function is currently not hooked up in any of Epics default subsystems, it is here for custom subsystems
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteAccepted(int32 LocalPlayerNum, FBPUniqueNetId PersonInvited, const FBlueprintSessionResult& SessionToJoin);


	// After a voice status has changed this event is triggered if the bEnableTalkingStatusDelegate property is true
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedVoice")
	void OnPlayerTalkingStateChanged(FBPUniqueNetId PlayerId, bool bIsTalking);

	void OnPlayerTalkingStateChangedMaster(TSharedRef<const FUniqueNetId> PlayerId, bool bIsTalking);

	FOnPlayerTalkingStateChangedDelegate PlayerTalkingStateChangedDelegate;
	FDelegateHandle PlayerTalkingStateChangedDelegateHandle;


	// Called when the designated LocalUser has changed login state
	UFUNCTION(BlueprintImplementableEvent , Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginChanged"))
	void OnPlayerLoginChanged(int32 PlayerNum);

	void OnPlayerLoginChangedMaster(int32 PlayerNum);
	FOnLoginChangedDelegate PlayerLoginChangedDelegate;
	FDelegateHandle PlayerLoginChangedDelegateHandle;

	// Called when the designated LocalUser has changed login status
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedIdentity", meta = (DisplayName = "OnPlayerLoginStatusChanged"))
	void OnPlayerLoginStatusChanged(int32 PlayerNum, EBPLoginStatus PreviousStatus, EBPLoginStatus NewStatus, FBPUniqueNetId NewPlayerUniqueNetID);

	void OnPlayerLoginStatusChangedMaster(int32 PlayerNum, ELoginStatus::Type PreviousStatus, ELoginStatus::Type NewStatus, const FUniqueNetId & NewPlayerUniqueNetID);
	FOnLoginStatusChangedDelegate PlayerLoginStatusChangedDelegate;
	FDelegateHandle PlayerLoginStatusChangedDelegateHandle;


	//*** Session Invite Received From Friend ***//
	// REMOVED BECAUSE IT NEVER GETS CALLED
	/*FOnSessionInviteReceivedDelegate SessionInviteReceivedDelegate;
	FDelegateHandle SessionInviteReceivedDelegateHandle;

	void OnSessionInviteReceivedMaster(const FUniqueNetId &InvitedPlayer, const FUniqueNetId &FriendInviting, const FOnlineSessionSearchResult& Session);

	// After a session invite has been sent from a friend this event is triggered, call JoinSession on the session result to join it
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnSessionInviteReceived(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &FriendInviting, const FBlueprintSessionResult &Session);
	*/

	//*** Friend Invite Accepted ***//
	/*FOnInviteAcceptedDelegate FriendInviteAcceptedDelegate;
	FDelegateHandle FriendInviteAcceptedDelegateHandle;

	void OnFriendInviteAcceptedDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &PlayerInvited);

	// After a session invite has been accepted by a friend this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnFriendInviteAccepted(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &PlayerInvited);
	*/

	//*** Friend Invite Rejected ***//
	/*FOnInviteRejectedDelegate SessionInviteRejectedByFriendDelegate;
	FDelegateHandle InviteRejectedByFriendDelegateHandle;

	void OnFriendInviteRejectedDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &PlayerDeclined);

	// After a friend invite has been rejected this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnFriendInviteRejected(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &PlayerDeclined);
	*/

	//*** Removed By Friend ***//
	/*FOnFriendRemovedDelegate RemovedByFriendDelegate;
	FDelegateHandle RemovedByFriendDelegateHandle;

	void OnRemovedByFriendDelegateMaster(const FUniqueNetId& LocalPlayer, const FUniqueNetId &FriendRemoved);

	// After a friend removed the player this event is triggered
	UFUNCTION(BlueprintImplementableEvent, Category = "AdvancedFriends")
	void OnRemovedByFriend(const FBPUniqueNetId &InvitedPlayer, const FBPUniqueNetId &FriendRemoved);*/
};

