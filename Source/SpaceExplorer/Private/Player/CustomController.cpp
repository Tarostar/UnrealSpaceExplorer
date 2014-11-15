

#include "SpaceExplorer.h"
#include "CustomPawn.h"
#include "InventoryObject.h"
#include "SpaceExplorerPawn.h"
#include "CustomController.h"

ACustomController::ACustomController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

// TODO: investigate and potentially implement
/*
bool ACustomController::SaveGameDataToFileCompressed(const FString& FullFilePath, int32& SaveDataInt32, FVector& SaveDataVector, TArray<FRotator>& SaveDataRotatorArray)
{
FBufferArchive ToBinary;
SaveLoadData(ToBinary, NumGemsCollected, PlayerLocation, ArrayOfRotationsOfTheStars);

//Pre Compressed Size
ClientMessage("~ PreCompressed Size ~");
ClientMessage(FString::FromInt(ToBinary.Num()));

//

// Compress File
//tmp compressed data array
TArray<uint8> CompressedData;
FArchiveSaveCompressedProxy Compressor =
FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

//Send entire binary array/archive to compressor
Compressor << ToBinary;

//send archive serialized data to binary array
Compressor.Flush();

//

//Compressed Size
ClientMessage("~ Compressed Size ~");
ClientMessage(FString::FromInt(CompressedData.Num()));


if (!FPlatformFileManager) return false;

//vibes to file, return successful or not
if (FFileHelper::SaveArrayToFile(CompressedData, *FullFilePath))
{
// Free Binary Arrays
Compressor.FlushCache();
CompressedData.Empty();

ToBinary.FlushCache();
ToBinary.Empty();

// Close Buffer
ToBinary.Close();

ClientMessage("File Save Success!");

return true;
//
}
else
{
// Free Binary Arrays
Compressor.FlushCache();
CompressedData.Empty();

ToBinary.FlushCache();
ToBinary.Empty();

// Close Buffer
ToBinary.Close();

ClientMessage("File Could Not Be Saved!");

return false;
//
}
}

*/

bool ACustomController::SaveGameDataToFile(const FString& FullFilePath, ACustomPawn * PlayerPawn)
{
	//note that the supplied FString must be the entire Filepath

	ASpaceExplorerPawn* pSpaceExplorerPawn = Cast<ASpaceExplorerPawn>(GetPawn());
	if (!pSpaceExplorerPawn)
	{
		// no player pawn - abort
		return false;
	}

	// Step 1: Variable Data -> Binary

	FBufferArchive ToBinary;
	SaveLoadData(ToBinary, PlayerPawn);

	// No Data
	if (ToBinary.Num() <= 0) 
		return false;

	// Step 2: Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		ClientMessage("Save Success!");
		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	ClientMessage("File Could Not Be Saved!");

	return false;
}

bool ACustomController::LoadGameDataFromFileCompressed(const FString& FullFilePath, ACustomPawn * PlayerPawn)
{
	// Load Compressed data array
	TArray<uint8> TheBinaryArray;
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		ClientMessage("FFILEHELPER:>> Invalid File");
		return false;
	}

	// Testing
	ClientMessage("Loaded File Size");
	ClientMessage(FString::FromInt(TheBinaryArray.Num()));

	// File Load Error
	if (TheBinaryArray.Num() <= 0) 
		return false;

	/* Read the Data Retrieved by GFileManager */

	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	SaveLoadData(FromBinary, PlayerPawn);
	
	/* Clean up */
	
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
//	TheBinaryArray.Close();

	return true;
}

void ACustomController::SaveLoadData(FArchive& Ar, ACustomPawn * PlayerPawn)
{
	if (PlayerPawn)
	{
		PlayerPawn->SaveLoad(Ar);
	}
}

int32 ACustomController::GetInventoryObjectCount()
{
	// TODO: this should work with any pawn - potentially we need to create a pawn class hierarchy
	ASpaceExplorerPawn* pSpaceExplorerPawn = Cast<ASpaceExplorerPawn>(GetPawn());
	if (pSpaceExplorerPawn)
		return pSpaceExplorerPawn->GetInventoryObjectCount();

	// no inventory
	return 0;
}

AInventoryObject* ACustomController::GetInventoryObjectFromID(int32 nID)
{
	// TODO: this should work with any pawn - potentially we need to create a pawn class hierarchy
	ASpaceExplorerPawn* pSpaceExplorerPawn = Cast<ASpaceExplorerPawn>(GetPawn());
	if (pSpaceExplorerPawn)
		return pSpaceExplorerPawn->GetInventoryObjectFromID(nID);
	
	// does not exist
	return nullptr;
}

AInventoryObject* ACustomController::GetInventoryObjectFromIndex(int32 nIndex)
{
	// TODO: this should work with any pawn - potentially we need to create a pawn class hierarchy
	ASpaceExplorerPawn* pSpaceExplorerPawn = Cast<ASpaceExplorerPawn>(GetPawn());
	if (pSpaceExplorerPawn)
		return pSpaceExplorerPawn->GetInventoryObjectFromIndex(nIndex);

	// does not exist
	return nullptr;
}

void ACustomController::InvokeAction(class DragObject& item)
{
	// TODO: this should work with any pawn - potentially we need to create a pawn class hierarchy
	ASpaceExplorerPawn* pSpaceExplorerPawn = Cast<ASpaceExplorerPawn>(GetPawn());
	if (pSpaceExplorerPawn)
		return pSpaceExplorerPawn->InvokeAction(item);
}
