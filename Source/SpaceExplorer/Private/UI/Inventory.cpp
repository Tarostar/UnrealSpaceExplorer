

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "CustomHUD.h"
#include "Inventory.h"
#include "ActionBar.h"

AInventory::AInventory(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bInvOpen = false;
	m_actionBar = nullptr;
	m_pHUD = nullptr;

	m_nWidthCount = -1;
	m_nHeightCount = -1;

	m_fSlotSize = 64.f;
	m_fInventoryBorder = 0.f;

	m_vInvStartpos = FVector2D(0.f, 0.f);

	m_vaInvHitBoxPositions.SetNum(m_nWidthCount * m_nHeightCount);

	static ConstructorHelpers::FObjectFinder<UTexture> HUDTextureOb(TEXT("/Game/HUD/Textures/InvDarkTransparent64"));
	m_texture = HUDTextureOb.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDTextureSelOb(TEXT("/Game/HUD/Textures/InvSel64"));
	m_textureSelected = HUDTextureSelOb.Object;

	m_nHoverIndex = -1;

	m_pInventory = nullptr;
	m_pPreviousInventory = nullptr;

	m_bInGroup = false;
}

void AInventory::Init(ACustomHUD * pHUD, AActionBar * pActionBar, float fSlotSize, float fInventoryBorder)
{
	m_pHUD = pHUD;
	m_actionBar = pActionBar;
	m_fSlotSize = fSlotSize;
	m_fInventoryBorder = fInventoryBorder;
}

bool AInventory::IsInvOpen()
{
	return m_bInvOpen;
}

void AInventory::ToggleInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory)
{
	if (m_bInvOpen)
	{
		m_bInvOpen = false;
		return;
	}

	OpenInventory(pInventory, bInGroup, pPreviousInventory);
}

void AInventory::CloseInventory()
{
	// TODO: any cleanup?

	m_bInvOpen = false;
}

void AInventory::OpenInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory)
{
	if (pInventory == nullptr)
	{
		return;
	}

	m_pPreviousInventory = pPreviousInventory;
		
	m_pInventory = pInventory;

	if (m_bInGroup != bInGroup || pInventory->InvWidthCount != m_nWidthCount || pInventory->InvHeightCount != m_nHeightCount)
	{
		// inventory group, width or height has changed
		m_nWidthCount = pInventory->InvWidthCount;
		m_nHeightCount = pInventory->InvHeightCount;
		m_bInGroup = bInGroup;

		m_vaInvHitBoxPositions.SetNum(m_nWidthCount * m_nHeightCount);
		UpdatePositions();
	}

	m_bInvOpen = true;
}

void AInventory::Move(bool bInGroup, AInventory* pPreviousInventory)
{
	m_bInGroup = bInGroup;
	m_pPreviousInventory = pPreviousInventory;
	UpdatePositions();
}

void AInventory::DrawInventory()
{
	if (m_pInventory == nullptr || !m_bInvOpen || m_vaInvHitBoxPositions.Num() != m_nWidthCount * m_nHeightCount)
	{
		return;
	}

	// loop backwards so "slot-boxes" are drawn behind large items spanning several slots
	int i;
	for (i = m_vaInvHitBoxPositions.Num() - 1; i >= 0; i--)
	{
		// hitbox
		FString name = FString::FromInt(m_pInventory->GetID()) + TEXT("_") + FString::FromInt(i);
		m_pHUD->AddHitBox(m_vaInvHitBoxPositions[i], FVector2D(m_fSlotSize, m_fSlotSize) * m_pHUD->GetCurrentRatio(), FName(*name), false);

		UTexture *texture = m_texture;
		if (m_nHoverIndex == i)
		{
			// selected
			texture = m_textureSelected;
		}

		// texture
		if (texture)
		{
			m_pHUD->DrawTextureSimple(texture, m_vaInvHitBoxPositions[i].X, m_vaInvHitBoxPositions[i].Y, m_pHUD->GetCurrentRatio());
		}

		// draw item texture (if item exists and is not being dragged)
		
		if (m_pInventory->HasItem(i))
		{
			if (m_pHUD->IsDragging() && m_pHUD->GetDraggedItemInventoryID() == m_pInventory->GetID() && m_pHUD->GetDraggedItemSlotIndex() == i)
			{
				// this item is currently being dragged so don't draw
				continue;
			}

			// check if item spans multiple slots and scale accordingly
			if (m_pInventory->InventorySlots[i]->GetItemWidth() > 1 || m_pInventory->InventorySlots[i]->GetItemHeight() > 1)
			{
				// only draw for the upper, left index
				int32 nUpperLeftIndex = m_pInventory->GetUpperLeft(i);
				if (nUpperLeftIndex == i)
				{
					// texture must be correctly sized to cover the right number of slots (i.e. 128 pixels per slot)
					m_pHUD->DrawTextureSimple(m_pInventory->InventorySlots[i]->m_inventoryTexture, m_vaInvHitBoxPositions[i].X, m_vaInvHitBoxPositions[i].Y, 0.5f * m_pHUD->GetCurrentRatio());
				}
			}
			else
			{
				// at the moment 128 pixels is the size of one slot for inventory items
				m_pHUD->DrawTextureSimple(m_pInventory->InventorySlots[i]->m_inventoryTexture, m_vaInvHitBoxPositions[i].X, m_vaInvHitBoxPositions[i].Y, 0.5f * m_pHUD->GetCurrentRatio());
			}
		}
	}
}

void AInventory::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}

void AInventory::SetStartPosition()
{
	if (m_pHUD == nullptr)
	{
		return;
	}

	// inventory width
	float fMargin = m_fSlotSize;
	float fInvWidth = m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nWidthCount;
	
	// default to first inventory drawing in top-right corner
	float x = 0;
	float y = 0;

	if (!m_bInGroup)
	{
		// single inventory opened - draw it above action bar on right side (minus its width and margin)

		x = m_pHUD->VScreenDimensions.X - fInvWidth - fMargin;

		if (m_actionBar && m_actionBar->IsVisible())
		{
			// inventory position above action bar
			y = m_actionBar->GetStartPos().Y - m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nHeightCount;
		}
		else
		{
			// inventory position above bottom of screen
			y = m_pHUD->VScreenDimensions.Y - m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nHeightCount;
		}
	}
	else if (m_pPreviousInventory)
	{
		// position after previous inventory if space, otherwise below

		// where previous inventory ends (+ margin)
		x = m_pPreviousInventory->m_vInvStartpos.X + m_pPreviousInventory->m_fSlotSize * m_pPreviousInventory->m_nWidthCount * m_pHUD->GetCurrentRatio() + fMargin;
		y = m_pPreviousInventory->m_vInvStartpos.Y;

		if (x + fInvWidth > m_pHUD->VScreenDimensions.X)
		{
			// does not fit, move down (at the moment we don't bother to check if fits on screen)
			x = 0;
			y = m_pPreviousInventory->m_vInvStartpos.Y + m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nHeightCount + fMargin;
		}
	}

	// store the start position for this inventory
	m_vInvStartpos = FVector2D(x, y);
}

void AInventory::SetHitBoxPositionArray()
{
	int i;
	for (i = 0; i < m_vaInvHitBoxPositions.Num(); i++)
	{
		int32 nColumn = i / m_nWidthCount;
		int32 nRow = i - nColumn * m_nWidthCount;

		float X = nRow * m_fSlotSize * m_pHUD->GetCurrentRatio() + m_vInvStartpos.X + m_fInventoryBorder * m_pHUD->GetCurrentRatio();
		float Y = nColumn * m_fSlotSize * m_pHUD->GetCurrentRatio() + m_vInvStartpos.Y + m_fInventoryBorder * m_pHUD->GetCurrentRatio();

		m_vaInvHitBoxPositions[i] = FVector2D(X, Y);
	}
}

bool AInventory::ItemDrag(bool bPickup, class DragObject& item)
{
	if (m_pInventory == nullptr)
	{
		// we have no inventory object associated - do nothing.
		return false;
	}

	if (!bPickup)
	{
		if (item.IsDragging())
		{	
			// item is being dragged

			if (m_nHoverIndex < 0)
			{
				// item is not being dropped into this inventory - do nothing.
				return false;
			}

			if (item.GetInventoryID() == m_pInventory->GetID())
			{
				// item being dragged FROM this inventory.

				if (item.GetSlotIndex() != m_nHoverIndex)
				{
					// TODO: this needs to replicate to/from server...
					// item is dropped into new slot in same inventory - move it.
					m_pInventory->MoveItem(item.GetSlotIndex(), m_nHoverIndex, true);
				}				
				
				// drop item being dragged
				item.Drop();
				return true;
			}

			// item being dragged from another inventory

			// TODO: this needs to replicate to/from server...
			
			// first retrieve it
			AInventoryObject* const pSourceInventory = m_pHUD->GetSourceInventoryObjectFromID(item.GetInventoryID());
			if (pSourceInventory == nullptr)
			{
				// can't get source inventory
				item.Drop();
				return true;
			}

			AItem * pItem = pSourceInventory->RetrieveItem(item.GetSlotIndex());
			if (pItem)
			{
				// we have an item to move - attempt to add it
				if (!m_pInventory->AddItem(m_nHoverIndex, pItem))
				{
					// failed to add item - try to put it back (if this fails we have a problem...)
					if (!pSourceInventory->AddItem(item.GetSlotIndex(), pItem))
					{
						// TODO: handle - we have an orphaned item... should not happen
					}
				}

				// drop item being dragged
				item.Drop();
				return true;
			}
		}
		
		// not us...
		return false;
	}

	if (!m_bInvOpen || m_nHoverIndex < 0)
	{
		// not this inventory
		return false;
	}

	// we have a hover index, so now we just need to check if slot has an item
	if (!m_pInventory->HasItem(m_nHoverIndex))
	{
		// no item (or potentially out-of-bounds)
		return false;
	}

	// start "dragging" item - which simply means tracking which item we were hovering over when drag started
	item.Init("", EActionType::Use, m_pInventory->GetID(), m_nHoverIndex);

	return true;
}

bool AInventory::CheckMouseOver(const FName BoxName, bool bBegin)
{
	// TODO: note - each inventory (there will eventually be multiple) must have its own unique BoxName
	// TODO: easiest achieved by having the first part of BoxName indicate inventory
	// i.e. "I0", "I1", etc. - see action bar for how to implemnet

	if (!m_bInvOpen || m_pInventory == nullptr)
	{
		// inventory is not open so we know it can't be a hitbox in this inventory
		return false;
	}

	// TODO: with support for multiple inventories this needs to get smarter and check we are in right inventory first
	// it also needs to store one reference to the current hitbox for all inventories in the CustomHUD

	FString strHitboxName = BoxName.ToString();
	int32 n = strHitboxName.Find(TEXT("_"));
	if (n <= 0)
	{
		// not formatted as an inventory hitbox name, e.g. <InventoryID>_<SlotIndex>
		return false;
	}
	
	if (strHitboxName.Len() <= n + 1 || !strHitboxName.Left(n).IsNumeric() || !strHitboxName.Mid(n + 1).IsNumeric())
	{
		// not formatted as an inventory hitbox name, e.g. <InventoryID>_<SlotIndex>
		return false;
	}

	// get inventory ID
	int nID = FCString::Atoi(*strHitboxName.Left(n));
	if (nID != m_pInventory->GetID())
	{
		// not this inventory
		return false;
	}

	// get inventory slot index number
	int nIndex = FCString::Atoi(*strHitboxName.Mid(n + 1));
	
	if (bBegin)
	{
		// yes - this inventory
		m_nHoverIndex = nIndex;
		return true;
	}

	// we check hover index to avoid slots flickering on/off when placed pixel-to-pixel (i.e. if BEGIN already called for another slot then ignore the "END" for all other slots.)
	if (m_nHoverIndex == nIndex)
	{
		// turn off current hover index
		m_nHoverIndex = -1;
		return true;
	}

	// not current hover index - ignore, but return true as we have handled it (if BoxName didn't belong to this inventory we would never have got here)
	return true;
}

int32 AInventory::GetID()
{
	if (m_pInventory)
	{
		return m_pInventory->GetID();
	}
	
	return -1;
}

bool AInventory::InvokeAction()
{
	if (!m_bInvOpen || m_nHoverIndex < 0 || !m_pHUD || !m_pInventory)
	{
		// not me (or invalid pointer)
		return false;
	}

	// we have a hover index, so now we just need to check if slot has an item
	if (!m_pInventory->HasItem(m_nHoverIndex))
	{
		// no item (or potentially out-of-bounds) - still this was us
		return true;
	}

	class DragObject item;
	item.Init("", EActionType::Use, m_pInventory->GetID(), m_nHoverIndex);

	m_pHUD->InvokeAction(item);

	return true;
}

bool AInventory::ActiveHitbox()
{
	if (!m_bInvOpen || !m_pHUD || !m_pInventory)
	{
		// not me (or invalid pointer)
		return false;
	}

	// do nothing, but confirm within hitbox
	return m_nHoverIndex >= 0;
}
