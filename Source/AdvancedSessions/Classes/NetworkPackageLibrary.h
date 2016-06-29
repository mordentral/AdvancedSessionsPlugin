// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OnlineSubSystemHeader.h"
#include "NetworkPackageLibrary.generated.h"


//General Advanced Sessions Log
DECLARE_LOG_CATEGORY_EXTERN(NetworkPackageLibraryLog, Log, All);

// A struct to contain information about a chunk send over network
USTRUCT()
struct FNetworkPackage
{
	GENERATED_BODY()
public:
	UPROPERTY()
		uint8 ThisPackageNumber;
	UPROPERTY()
		uint8 TotalExpectedPackages;
	UPROPERTY()
		TArray<uint64> ChunkData;
	UPROPERTY()
		uint32 PackageID;
	UPROPERTY()
		uint32 StartLocation;
	UPROPERTY()
		uint32 PackageSize;
	UPROPERTY()
		uint32 TotalDataSize;
	UPROPERTY()
		uint8 PackageType;

	FNetworkPackage()
	{
		ThisPackageNumber = 0;
		TotalExpectedPackages = 0;
		PackageID = 0;
		PackageType = 0;
	}
};

USTRUCT()
struct FNetworkPackageBin
{
	GENERATED_BODY()
public:
	TArray<uint8> RemainingPackageNumbers;
	//TArray<FNetworkPackage> ChunkDataBin;
	uint8 TotalExpectedPackages;
	TArray<uint8> ChunkData;
	int32 PackageID;
	uint8 PackageType;
	uint32 TotalDataSize;

	bool bFinishedReceiving;

	FNetworkPackageBin()
	{
		TotalExpectedPackages = 0;
		bFinishedReceiving = false;
		PackageID = 0;
		TotalDataSize = 0;
		PackageType = 0;
	}

	FNetworkPackageBin(const FNetworkPackage & Package)
	{
		bFinishedReceiving = false;
		TotalExpectedPackages = 0;
		InitializeFromPackage(Package);
	}

	void InitializeFromPackage(const FNetworkPackage & Package)
	{
		for (int i = 0; i < Package.TotalExpectedPackages; ++i)
			RemainingPackageNumbers.Add(i + 1);

		ChunkData.AddUninitialized(Package.TotalDataSize);

		FMemory::Memcpy(ChunkData.GetData() + Package.StartLocation, Package.ChunkData.GetData(), Package.PackageSize);

		TotalDataSize = Package.TotalDataSize;
		PackageType = Package.PackageType;

		TotalExpectedPackages = Package.TotalExpectedPackages;
		PackageID = Package.PackageID;
	}

	// Returns true if this was the last package it was waiting on
	bool AddPackage(const FNetworkPackage & Package)
	{
		if (Package.ThisPackageNumber <= TotalExpectedPackages && RemainingPackageNumbers.Find(Package.ThisPackageNumber) == INDEX_NONE)
		{
			RemainingPackageNumbers.Remove(Package.ThisPackageNumber);

			FMemory::Memcpy(ChunkData.GetData() + Package.StartLocation, Package.ChunkData.GetData(), Package.PackageSize);

			if (!RemainingPackageNumbers.Num())
			{
				bFinishedReceiving = true;
				UncompressData();
				return true;
			}
		}
		else
		{
			return false; // Already received this package, something is wrong
		}

		return false;
	}

	void UncompressData()
	{
		FBufferArchive DecompressedBinaryArray;
		FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(ChunkData, ECompressionFlags::COMPRESS_ZLIB);

		if (Decompressor.GetError())
		{
			// File was not compressed
			return;
		}

		Decompressor << DecompressedBinaryArray;
		Decompressor.FlushCache();
		Decompressor.Close();

		ChunkData.Empty();
		DecompressedBinaryArray << ChunkData;
	}
};


// An actor component meant to handle sending large data array's across the network
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class UNetworkPackageLibraryComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
	~UNetworkPackageLibraryComponent();

public:
#pragma region NETWORKING Networking Region

	// NETWORKING FUNCTIONS
	//UPROPERTY(Replicated, BlueprintReadOnly)
	int32 IncrementalPackageID;

	int32 GetPackageID() { return IncrementalPackageID++; }

	TArray<FNetworkPackageBin> NetworkPackageQueue;

	UFUNCTION(BlueprintCallable, Category = "NetworkPackageLibrary")
	bool DeleteQueuedDataPackage(int32 NetworkPackageID)
	{
		for (int i = 0; i < NetworkPackageQueue.Num(); ++i)
		{
			if (NetworkPackageQueue[i].PackageID == NetworkPackageID)
			{
				NetworkPackageQueue.RemoveAt(i);
				return true;
			}
		}

		return false;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "NetworkPackageLibrary")
	void BPEventNotifyPackageReady(int32 PackageID, uint8 PackageType);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ReceiveChunkDataPackage(FNetworkPackage Package);

	UFUNCTION(BlueprintCallable, Category = "NetworkPackageLibrary")
	virtual void NetworkReplicateChunkData_ToServer(TArray<uint8> ChunkData, uint8 PackageType)
	{
		// I can up this to 32 byte structures instead of the 8 bytes structure to get up to 64 KB per packet, right now it is 16kb per packet max with the 64bit size
		int MAX_ARRAY_SIZE = 2048;

		FNetworkPackage newPackage;
		newPackage.PackageID = GetPackageID();

		FBufferArchive ToBinary;
		ToBinary << ChunkData;

		// Compress File 
		TArray<uint8> CompressedData;
		FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

		//Send entire binary array/archive to compressor
		Compressor << ToBinary;

		//send archive serialized data to binary array
		Compressor.Flush();
		Compressor.FlushCache();
		Compressor.Close();

		uint32 numPackages = FMath::CeilToInt(CompressedData.Num() / MAX_ARRAY_SIZE);
		if ((CompressedData.Num() % MAX_ARRAY_SIZE) > 0)
			numPackages++;

		newPackage.TotalDataSize = CompressedData.Num();
		newPackage.TotalExpectedPackages = numPackages;
		newPackage.PackageType = PackageType;

		uint32 curLoc = 0;

		for (uint32 i = 0; i < numPackages; ++i)
		{
			if (curLoc >= (uint32)CompressedData.Num())
				return; // End here, no more to send

			FNetworkPackage pack = newPackage;
			pack.ThisPackageNumber = i;
			pack.StartLocation = curLoc;

			uint32 thisCount = FMath::Clamp<uint32>((uint32)(newPackage.TotalDataSize - curLoc), 0, MAX_ARRAY_SIZE);
			pack.ChunkData.AddUninitialized(FMath::CeilToInt(thisCount / sizeof(uint64)));
			pack.PackageSize = thisCount;
			FMemory::Memcpy(pack.ChunkData.GetData(), CompressedData.GetData() + curLoc, thisCount);

			// Send it up the pipeline
			ReceiveChunkDataPackage(pack);
			//TargetController->ReceiveChunkDataPackage(this, pack);
			curLoc += MAX_ARRAY_SIZE;
		}
	}

	// END NETWORKING FUNCTIONS

#pragma endregion End of networking Region

};	
