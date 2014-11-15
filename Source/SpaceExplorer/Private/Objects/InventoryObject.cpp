

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "UsableObject.h"

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

bool AInventoryObject::AddItemFirstAvailableSlot(AUsableObject* pItem)
{
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

bool AInventoryObject::AddItem(int32 nIndex, AUsableObject* pItem)
{
	if (CheckItemFits(nIndex, pItem->m_nInvHeight, pItem->m_nInvWidth))
	{
		// item fits, insert it (this should never fail unless there is a bug)
		return InsertItem(nIndex, pItem);
	}

	// item did not fit
	return false;
}

AUsableObject* AInventoryObject::ReplaceItem(int32 nIndex, AUsableObject* pItem)
{
	// retrieve any item in target slot which will set the slots to nullptr
	int32 nUpperLeft;
	AUsableObject * pTargetItem = RetrieveItem(nIndex, nUpperLeft);
	
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

AUsableObject* AInventoryObject::RetrieveItem(int32 nIndex)
{
	int32 nUpperLeft;
	return RetrieveItem(nIndex, nUpperLeft);
}

AUsableObject* AInventoryObject::RetrieveItem(int32 nIndex, int32& nUpperLeft)
{
	if (!GetUpperLeft(nIndex, nUpperLeft))
	{
		// no item selected (or invalid index)
		return nullptr;
	}

	AUsableObject * pItem = InventorySlots[nUpperLeft];
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

AUsableObject* AInventoryObject::CloneItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// create a copy and return it
	return new AUsableObject(*InventorySlots[nIndex]);

}

AUsableObject* AInventoryObject::GetItem(int32 nIndex)
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
	AUsableObject * pSourceItem = RetrieveItem(nFrom, nFromUpperLeft);
	if (pSourceItem == nullptr)
	{
		// no item found
		return false;
	}

	// retrieve any item in target slot which will set the slots to nullptr
	int32 nToUpperLeft;
	AUsableObject * pTargetItem = RetrieveItem(nTo, nToUpperLeft);
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
	AUsableObject * pItem = RetrieveItem(nIndex);
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

bool AInventoryObject::GetUpperLeft(int32 nIndex, int32& nUpperLeftIndex)
{
	if (nIndex < 0 || nIndex >= InventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// get item to check for when looking for upper left
	AUsableObject * pItem = InventorySlots[nIndex];

	if (pItem == nullptr)
	{
		// no item selected
		return false;
	}

	// start on selected index
	nUpperLeftIndex = nIndex;

	if (pItem->m_nInvWidth == 1 && pItem->m_nInvHeight == 1)
	{
		// exactly one slot - so done
		return true;
	}

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

	return true;
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

bool AInventoryObject::InsertItem(int32 nIndex, AUsableObject* pItem)
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
		if (Ar.IsLoading())
		{
			
		}
		else
		{
			if (InventorySlots[i])
			{
				InventorySlots[i]->SaveLoad(Ar);
			}
			else
			{
				Ar << nullptr;
			}
		}
	}
}