// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSubSystemHeader.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Online.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionInterface.h"

#include "AdvancedSteamWorkshopLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(AdvancedSteamWorkshopLog, Log, All);


// Using a custom struct because uint32 isn't blueprint supported and I don't want to cast to int32
// due to the size of the workshop it could end up overflowing?
USTRUCT(BlueprintType)
struct FBPSteamWorkshopID
{
	GENERATED_USTRUCT_BODY()

public:

	uint32 SteamWorkshopID;

	FBPSteamWorkshopID()
	{

	}

	FBPSteamWorkshopID(uint32 ID)
	{
		SteamWorkshopID = ID;
	}
};


UCLASS()
class UAdvancedSteamWorkshopLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	//********* Steam Functions *************//
	
	// Returns IDs for subscribed workshop items, TArray length dictates how many
	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static TArray<FBPSteamWorkshopID> GetSubscribedWorkshopItems(int32 & NumberOfItems);

	UFUNCTION(BlueprintCallable, Category = "Online|AdvancedSteamWorkshop")
	static void GetNumSubscribedWorkshopItems(int32 & NumberOfItems);

};	
