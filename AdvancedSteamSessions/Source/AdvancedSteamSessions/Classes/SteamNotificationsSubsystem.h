// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED

#include <steam/steam_api.h>

#endif

#include "SteamNotificationsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSteamOverlayActivated, bool, bOverlayState);

UCLASS()
class ADVANCEDSTEAMSESSIONS_API USteamNotificationsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	// Event thrown when the steam overlay switches states
	UPROPERTY(BlueprintAssignable, Category = "SteamEvents")
		FOnSteamOverlayActivated OnSteamOverlayActivated_Bind;
	
	USteamNotificationsSubsystem() : Super()
	{

	}

	class cSteamEventsStore
	{
	public:
		USteamNotificationsSubsystem* ParentSubsystem = nullptr;
		void Initialize(USteamNotificationsSubsystem* MyParent)
		{
			ParentSubsystem = MyParent;
		}

#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED
		cSteamEventsStore() :
		OnExternalUITriggeredCallback(this, &cSteamEventsStore::OnExternalUITriggered)
		{

		}
#else
		//cSteamEventsStore()
		//{

		//}
#endif

		//~cSteamEventsStore(){}

	private:
#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED
		STEAM_CALLBACK(cSteamEventsStore, OnExternalUITriggered, GameOverlayActivated_t, OnExternalUITriggeredCallback);
#endif
	};

	cSteamEventsStore MyEvents;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override
	{
		MyEvents.Initialize(this);
	}

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override
	{

	}
};

#if (PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX) && STEAM_SDK_INSTALLED
void USteamNotificationsSubsystem::cSteamEventsStore::OnExternalUITriggered(GameOverlayActivated_t* CallbackData)
{
	if (ParentSubsystem)
	{
		ParentSubsystem->OnSteamOverlayActivated_Bind.Broadcast((bool)CallbackData->m_bActive);
	}
}
#endif