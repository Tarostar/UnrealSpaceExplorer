

#pragma once

class ACustomPawn;
#include "GameFramework/PlayerController.h"
#include "CustomController.generated.h"

/**
 * Custom controller for custom player handling
 *
 */
UCLASS()
class SPACEEXPLORER_API ACustomController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:

	/** PUBLIC FUNCTIONS */

	// LOAD AND SAVE

	bool SaveGameDataToFile(const FString& FullFilePathy, ACustomPawn * PlayerPawn);
	bool LoadGameDataFromFileCompressed(const FString& FullFilePath, ACustomPawn * PlayerPawn);
	/*bool SaveGameDataToFileCompressed(const FString& FullFilePath, int32& SaveDataInt32, FVector& SaveDataVector, TArray<FRotator>& SaveDataRotatorArray);*/

	// INVENTORY OBJECTS and ITEMS

	/* Get an inventory based on its index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AInventoryObject* GetInventoryObjectFromIndex(int32 index);

	/* Get an inventory based on its ID */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AInventoryObject* GetInventoryObjectFromID(int32 nID);

	/* Get number of inventories owned by this player */
	int32 GetInventoryObjectCount();

	/* invoke an action (item optional) */
	void InvokeAction(class DragObject& item);

	/** PUBLIC VARIABLES */


private:
	/** PRIVATE FUNCTIONS */

	// FArchive is shared base class for FBufferArchive and FMemoryReader
	void SaveLoadData(FArchive& Ar, ACustomPawn * PlayerPawn);

	/** PRIVATE VARIABLES */
};


