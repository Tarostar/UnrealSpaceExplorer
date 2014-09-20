

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
	int32 m_nInvWidthCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 m_nInvHeightCount;

	// the array which represents the slots of the inventory (use a single dimension, and then width and height to derive actual layout and positions)
	// each slot points to an item, and an item that can take up multiple slots will have multiple slots pointing to that same item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	TArray<AUsableObject*> m_inventorySlots;

	/* Initialise inventory with ID and number of slots wide and high*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void Init(int32 id, int32 nWidth, int32 nHeight);

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
	
	/* remove and return item from inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
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

private:
	// unique ID
	int32 m_ID;

	bool GetUpperLeft(int32 nIndex, int32& nUpperLeftIndex);
	bool CheckItemFits(int32 nIndex, int32 nHeight, int32 nWidth);
	bool InsertItem(int32 nIndex, AUsableObject* pItem);
};
