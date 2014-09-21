

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "UsableObject.h"

AInventoryObject::AInventoryObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_nInvWidthCount = 10;
	m_nInvHeightCount = 5;
	m_ID = -1;
}

void AInventoryObject::Init(int32 id, int32 nWidth, int32 nHeight)
{
	m_ID = id;
	m_nInvWidthCount = nWidth;
	m_nInvHeightCount = nHeight;

	// TODO: can we just zero it instead?
	m_inventorySlots.Reset(nWidth * nHeight);
	for (int i = 0; i < m_inventorySlots.Num(); i++)
	{
		m_inventorySlots[i] = nullptr;
	}
}

bool AInventoryObject::AddItemFirstAvailableSlot(AUsableObject* pItem)
{
	for (int i = 0; i < m_inventorySlots.Num(); i++)
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
	if (nIndex < 0 || nIndex >= m_inventorySlots.Num())
	{
		// invalid index
		return false;
	}

	if (m_inventorySlots[nIndex] == nullptr)
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

	AUsableObject * pItem = m_inventorySlots[nUpperLeft];
	for (int nRow = 0; nRow < pItem->m_nInvHeight; nRow++)
	{
		// get start index for each row
		int nCurIndex = nUpperLeft + nRow * m_nInvWidthCount;
		for (int nCol = 0; nCol < pItem->m_nInvWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= m_inventorySlots.Num())
			{
				// error - went off screen, abort!
				GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, TEXT("Error - RetrieveItem: Item index is outside inventory array"));
				return pItem;
			}

			// set slot to empty
			m_inventorySlots[nCurIndex] = nullptr;
		}
	}

	// return item
	return pItem;
}

AUsableObject* AInventoryObject::CloneItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= m_inventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// create a copy and return it
	return new AUsableObject(*m_inventorySlots[nIndex]);

}

AUsableObject* AInventoryObject::GetItem(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= m_inventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// return pointer
	return m_inventorySlots[nIndex];

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
	if (nIndex < 0 || nIndex >= m_inventorySlots.Num())
	{
		// invalid index
		return false;
	}

	// get item to check for when looking for upper left
	AUsableObject * pItem = m_inventorySlots[nIndex];

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
	while (nUpperLeftIndex > 0 && pItem->m_nInvWidth > 1 && m_inventorySlots[nUpperLeftIndex - 1] == pItem)
	{
		// go left
		nUpperLeftIndex--;
	}

	// we know we are in leftmost column, so go up rows (if item spans several rows)	
	while (nUpperLeftIndex - m_nInvWidthCount >= 0 && pItem->m_nInvHeight > 1 && m_inventorySlots[nUpperLeftIndex - m_nInvWidthCount] == pItem)
	{
		// go up one row
		nUpperLeftIndex -= m_nInvWidthCount;
	}

	return true;
}

bool AInventoryObject::CheckItemFits(int32 nIndex, int32 nHeight, int32 nWidth)
{
	if (nIndex < 0 || nIndex >= m_inventorySlots.Num())
	{
		// invalid index - this is not an error, but means item cannot be added to inventory
		return false;
	}

	// check that there are enough free slots

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		// get start index for each row
		int nCurIndex = nIndex + nRow * m_nInvWidthCount;
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= m_inventorySlots.Num())
			{
				// invalid index - this is not an error, but means item cannot be added to inventory
				return false;
			}

			if (m_inventorySlots[nCurIndex] != nullptr)
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
		int nCurIndex = nIndex + nRow * m_nInvWidthCount;
		for (int nCol = 0; nCol < pItem->m_nInvWidth; nCol++)
		{
			nCurIndex += nCol;
			if (nCurIndex >= m_inventorySlots.Num())
			{
				GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, TEXT("Error - InsertItem: Item index is outside inventory array!"));
				return false;
			}

			m_inventorySlots[nCurIndex] = pItem;
		}
	}

	return true;
}

int32 AInventoryObject::GetID()
{
	return m_ID;
}

