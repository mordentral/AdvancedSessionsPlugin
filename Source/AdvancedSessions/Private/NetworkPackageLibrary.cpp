// Fill out your copyright notice in the Description page of Project Settings.
#include "OnlineSubSystemHeader.h"
#include "NetworkPackageLibrary.h"

//General Log
DEFINE_LOG_CATEGORY(NetworkPackageLibraryLog);

UNetworkPackageLibraryComponent::UNetworkPackageLibraryComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

//=============================================================================
UNetworkPackageLibraryComponent::~UNetworkPackageLibraryComponent()
{
}

bool UNetworkPackageLibraryComponent::ReceiveChunkDataPackage_Validate(FNetworkPackage Package)
{
	return true;
}

void UNetworkPackageLibraryComponent::ReceiveChunkDataPackage_Implementation(FNetworkPackage Package)
{
	for (int i = 0; i < NetworkPackageQueue.Num(); ++i)
	{
		if (NetworkPackageQueue[i].PackageID == Package.PackageID)
		{
			if (NetworkPackageQueue[i].AddPackage(Package))
			{
				if (NetworkPackageQueue[i].bFinishedReceiving)
					BPEventNotifyPackageReady(i, NetworkPackageQueue[i].PackageType);
				//else
				// NOTIFY BAD SEND
			}
			return;
		}
	}

	// Else need to add a new package
	FNetworkPackageBin newBin(Package);
	NetworkPackageQueue.Add(Package);
}
