

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "Item.h"

AInventoryObject::AInventoryObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	InvWidthCount = 10;
	InvHeightCount = 5;
	ID = -1;

	// bReplicates flag set to true:
	bReplicates = true;
}

void AInventoryObject::Init(int32 Id, int32 Width, int32 Height)
{
	ID = Id;
	InvWidthCount = Width;
	InvHeightCount = Height;
	ResetSlots();
}

void AInventoryObject::ResetSlots()
{
	InventorySlots.SetNum(InvWidthCount * InvHeightCount);
	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		InventorySlots[i] = nullptr;
	}
}

bool AInventoryObject::AddItemFirstAvailableSlot(AItem* pItem)
{
	if (pItem == nullptr)
	{
		return false;
	}

	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		if (AddItem(i, pItem))
		{
			// found slot space
			return true;
		}
	}

	// did not find available slots
	return false;
}

bool AInventoryObject::AddItem(int32 nIndex, AItem* pItem)
{
	if (CheckItemFits(nIndex, pItem->m_nInvHeight, pItem->m_nInvWidth))
	{
		// item fits, insert it (this should never fail unless there is a bug)
		return InsertItem(nIndex, pItem);
	}

	// item did not fit
	return false;
}

AItem* AInventoryObject::ReplaceItem(int32 nIndex, AItem* pItem)
{
	// retrieve any item in target slot which will set the slots to nullptr
	int32 nUpperLeft;
	AItem * pTargetItem = RetrieveItem(nIndex, nUpperLeft);
	
	// insert new item
	if (!AddItem(nIndex, pItem))
	{
		// failed to put item into target position
		if (pTargetItem != nullptr)
		{
			// we have target item - return to original position
			AddItem(nUpperLeft, pTargetItem);
		}

		return nullptr;
	}

	// success
	return pTargetItem; // may be nullptr	
}

bool AInventoryObject::HasItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return false;
	}

	if (InventorySlots[nIndex] == nullptr)
	{
		return false;
	}

	return true;
}

AItem* AInventoryObject::RetrieveItem(int32 nIndex)
{
	int32 nUpperLeft;
	return RetrieveItem(nIndex, nUpperLeft);
}

AItem* AInventoryObject::RetrieveItem(int32 nIndex, int32& nUpperLeft)
{	
	nUpperLeft = GetUpperLeft(nIndex);
	if (nUpperLeft == -1)
	{
		// no item selected (or invalid index)
		return nullptr;
	}

	AItem * pItem = InventorySlots[nUpperLeft];
	for (int nRow = 0; nRow < pItem->m_nInvHeight; nRow++)
	{
		// get start index for each row
		int nCurIndex = nUpperLeft + nRow * InvWidthCount;
		for (int nCol = 0; nCol < pItem->m_nInvWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= InventorySlots.Num())
			{
				// error - went off screen, abort!
				GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, TEXT("Error - RetrieveItem: Item index is outside inventory array"));
				return pItem;
			}

			// set slot to empty
			InventorySlots[nCurIndex] = nullptr;
		}
	}

	// return item
	return pItem;
}

AItem* AInventoryObject::CloneItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// create a copy and return it
	return new AItem(*InventorySlots[nIndex]);

}

AItem* AInventoryObject::GetItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// return pointer
	return InventorySlots[nIndex];

}

bool AInventoryObject::MoveItem(int32 nFrom, int32 nTo, bool bSwap)
{
	// retrieve item which will set the slots to nullptr
	int32 nFromUpperLeft;
	AItem * pSourceItem = RetrieveItem(nFrom, nFromUpperLeft);
	if (pSourceItem == nullptr)
	{
		// no item found
		return false;
	}

	// retrieve any item in target slot which will set the slots to nullptr
	int32 nToUpperLeft;
	AItem * pTargetItem = RetrieveItem(nTo, nToUpperLeft);
	if (bSwap && pTargetItem != nullptr)
	{
		// put target item in source item position
		if (!AddItem(nFromUpperLeft, pTargetItem))
		{
			// failed, just put source and target items back
			if (!AddItem(nToUpperLeft, pTargetItem))
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Could not return target item to slot: ") + FString::FromInt(nTo));

			if (!AddItem(nFromUpperLeft, pSourceItem))
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Could not return source item to slot: ") + FString::FromInt(nFrom));

			return false;
		}
	}
	
	// now move source item
	if (!AddItem(nTo, pSourceItem))
	{
		// failed to put source item into target position
		if (pTargetItem != nullptr)
		{
			// we have target item
			if (bSwap)
			{
				// retrieve target item (we already have a pointer to it)
				RetrieveItem(nFromUpperLeft);
			}

			// return target item to original position
			AddItem(nToUpperLeft, pTargetItem);
		}

		// return source item to original position
		AddItem(nFromUpperLeft, pSourceItem);
		return false;
	}

	// successfully moved source item
	if (!bSwap && pTargetItem != nullptr)
	{
		// TODO: we just let the Unreal garbage collector deal with this - is this ok?
		// did not swap, so delete target item
		// delete pTargetItem;
	}

	return true;
}

bool AInventoryObject::DestroyItem(int32 nIndex)
{
	// retrieve item which will set the slots to nullptr
	AItem * pItem = RetrieveItem(nIndex);
	if (pItem == nullptr)
	{
		// no item found
		return false;
	}

	// now delete the item
	// TODO: we just let the Unreal garbage collector deal with this - is this ok?
	// delete pItem;
	return true;
}

int32 AInventoryObject::GetUpperLeft(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return -1;
	}

	// get item to check for when looking for upper left
	AItem * pItem = InventorySlots[nIndex];

	if (pItem == nullptr)
	{
		// no item selected
		return -1;
	}

	if (pItem->m_nInvWidth == 1 && pItem->m_nInvHeight == 1)
	{
		// exactly one slot - so done
		return nIndex;
	}

	// multi-slot item, so start on selected index and iterate
	int32 nUpperLeftIndex = nIndex;

	// find leftmost column (if item spans several columns)
	while (nUpperLeftIndex > 0 && pItem->m_nInvWidth > 1 && InventorySlots[nUpperLeftIndex - 1] == pItem)
	{
		// go left
		nUpperLeftIndex--;
	}

	// we know we are in leftmost column, so go up rows (if item spans several rows)	
	while (nUpperLeftIndex - InvWidthCount >= 0 && pItem->m_nInvHeight > 1 && InventorySlots[nUpperLeftIndex - InvWidthCount] == pItem)
	{
		// go up one row
		nUpperLeftIndex -= InvWidthCount;
	}

	return nUpperLeftIndex;
}

bool AInventoryObject::CheckItemFits(int32 nIndex, int32 nHeight, int32 nWidth)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index - this is not an error, but means item cannot be added to inventory
		return false;
	}

	// check that there are enough free slots

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		// get start index for each row
		int nCurIndex = nIndex + nRow * InvWidthCount;
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= InventorySlots.Num())
			{
				// invalid index - this is not an error, but means item cannot be added to inventory
				return false;
			}

			if (InventorySlots[nCurIndex] != nullptr)
			{
				// not enough empty slots
				return false;
			}
		}
	}

	// sufficient empty slots
	return true;
}

bool AInventoryObject::InsertItem(int32 nIndex, AItem* pItem)
{
	for (int nRow = 0; nRow < pItem->m_nInvHeight; nRow++)
	{
		// get start index for each row
		int nCurIndex = nIndex + nRow * InvWidthCount;
		for (int nCol = 0; nCol < pItem->m_nInvWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= InventorySlots.Num())
			{
				GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, TEXT("Error - InsertItem: Item index is outside inventory array!"));
				return false;
			}

			InventorySlots[nCurIndex] = pItem;
		}
	}

	return true;
}

int32 AInventoryObject::GetID()
{
	return ID;
}

void AInventoryObject::SaveLoad(FArchive& Ar)
{
	
	Ar << InvWidthCount;
	Ar << InvHeightCount;

	if (Ar.IsLoading())
	{
		// If loading reset slots to be empty before loading items
		ResetSlots();

	}
	
	// Save/load items in inventory
	for (int i = 0; i < InventorySlots.Num(); i++)
	{
		// track if there is something in the inventory slot
		bool bSlot = false;
		if (Ar.IsLoading())
		{
			Ar << bSlot;
			if (bSlot)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Loading existing slot: ") + FString::FromInt(i));

				// load a new item into the world
				UWorld* const World = GetWorld();
				check(World);
				AItem * Item = World->SpawnActor<AItem>(AItem::StaticClass());
				check(Item);
				Item->SaveLoad(Ar);
				// add item back in - will fill all relevant slots
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Add Item: ") + FString::FromInt(i));
				AddItem(i, Item);
			}
		}	
		else
		{
			// saving - update is slot exists and is root index of item (i.e. upper left) as we only save an item once for root index
			bSlot = HasItem(i) && GetUpperLeft(i) == i;
			if (bSlot)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, TEXT("HasItem: ") + FString::FromInt(i));

				check(InventorySlots[i]);
				// root index of item in inventory - save this
				Ar << bSlot;
				InventorySlots[i]->SaveLoad(Ar);
			}
			else
			{
				
				// store that this slot is empty (or not root index of item)
				Ar << bSlot;
			}
		}
	}
}
