// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerState.h"

//#include "UObjectIterator.h"

#include "AdvancedGameSession.generated.h"




/**
 A quick wrapper around the game session to add a partial ban implementation. Just bans for the duration of the current session
*/
UCLASS(config = Game, notplaceable)
class AAdvancedGameSession : public AGameSession
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Transient)
	TMap<FUniqueNetIdRepl, FText> BanList;

	virtual bool BanPlayer(class APlayerController* BannedPlayer, const FText& BanReason)
	{

		if (APlayerState* PlayerState = (BannedPlayer != NULL) ? BannedPlayer->PlayerState : NULL)
		{
			FUniqueNetIdRepl UniqueNetID = PlayerState->GetUniqueId();
			bool bWasKicked = KickPlayer(BannedPlayer, BanReason);

			if (bWasKicked)
			{
				BanList.Add(UniqueNetID, BanReason);
			}

			return bWasKicked;
		}
		
		return false;
	}

	// This should really be handled in the game mode asking game session, but I didn't want to force a custom game session AND game mode
	// If done in the game mode, we could check prior to actually spooling up any player information in ApproveLogin
	virtual void PostLogin(APlayerController* NewPlayer) override
	{
		if (APlayerState* PlayerState = (NewPlayer != NULL) ? NewPlayer->PlayerState : NULL)
		{
			FUniqueNetIdRepl UniqueNetID = PlayerState->GetUniqueId();

			if (BanList.Contains(UniqueNetID))
			{
				KickPlayer(NewPlayer, BanList[UniqueNetID]);
			}
		}
	}
};

AAdvancedGameSession::AAdvancedGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}