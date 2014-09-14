

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

	UFUNCTION(BlueprintCallable, Category = Inventory)
	void Init(int32 id, int32 nWidth, int32 nHeight);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItemFirstAvailableSlot(AUsableObject* pItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItem(int32 nIndex, AUsableObject* pItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject * ReplaceItem(int32 nIndex, AUsableObject* pItem);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasItem(int32 nIndex);
	
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject * RetrieveItem(int32 nIndex);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	AUsableObject * CloneItem(int32 nIndex);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool MoveItem(int32 nFrom, int32 nTo, bool bSwap = true);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool DestroyItem(int32 nIndex);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetID();

private:
	// unique ID
	int32 m_ID;

	bool GetUpperLeft(int32 nIndex, int32& nUpperLeftIndex);
	bool CheckItemFits(int32 nIndex, int32 nHeight, int32 nWidth);
	bool InsertItem(int32 nIndex, AUsableObject* pItem);
};
