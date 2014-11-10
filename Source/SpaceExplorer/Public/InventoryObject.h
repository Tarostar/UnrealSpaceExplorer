

#pragma once

#include "UsableObject.h"
#include "GameFramework/Actor.h"
#include "InventoryObject.generated.h"

/**
 * Base class for inventory objects such as bags, sacks, containers and anything that can hold items
 */
UCLASS()
class SPACEEXPLORER_API AInventoryObject : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 InvWidthCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 InvHeightCount;

	// the array which represents the slots of the inventory (use a single dimension, and then width and height to derive actual layout and positions)
	// each slot points to an item, and an item that can take up multiple slots will have multiple slots pointing to that same item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TArray<AUsableObject*> InventorySlots;

	/* Initialise inventory with ID and number of slots wide and high */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void Init(int32 Id, int32 Width, int32 Height);

	/* Resets inventory slot array size and pointers */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void ResetSlots();

	/* Add item to first available slot */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItemFirstAvailableSlot(AUsableObject* pItem);

	/* Add item to specified index if fits - otherwise return false*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItem(int32 nIndex, AUsableObject* pItem);

	/* insert item in target index if fits and return any item which was there - otherwise return false*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject* ReplaceItem(int32 nIndex, AUsableObject* pItem);

	/* check if slot has an item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasItem(int32 nIndex);
	
	/* remove and return item from inventory, and update index to point to upper, left corner of any object spanning multiple slots*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject* RetrieveItem(int32 nIndex, int32& nUpperLeft);

	// UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject* RetrieveItem(int32 nIndex);

	/* make a copy of item from inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject* CloneItem(int32 nIndex);

	/* get a pointer to item in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject* GetItem(int32 nIndex);

	/* move item from one slot position to another (if fits) - also move target item if swapping (and fits), if not swapping delete any item at target index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool MoveItem(int32 nFrom, int32 nTo, bool bSwap = true);

	/* remove item at target index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool DestroyItem(int32 nIndex);

	/* get unique ID for inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetID();

	// start at index and find upper, left index
	// if found return true and upperLeft index
	bool GetUpperLeft(int32 nIndex, int32& nUpperLeftIndex);

	void SaveLoad(FArchive& Ar);

private:
	// unique ID
	int32 ID;

	bool CheckItemFits(int32 nIndex, int32 nHeight, int32 nWidth);
	bool InsertItem(int32 nIndex, AUsableObject* pItem);
};
