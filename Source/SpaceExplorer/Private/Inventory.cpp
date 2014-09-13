

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "CustomHUD.h"
#include "Inventory.h"

AInventory::AInventory(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bInvOpen = false;
	m_pHotbar = NULL;
	m_pHUD = NULL;

	m_nWidthCount = 10;
	m_nHeightCount = 5;

	m_fSlotSize = 64.f;
	m_fInventoryBorder = 0.f;

	m_vInvStartpos = FVector2D(0.f, 0.f);

	m_vaInvHitBoxPositions.SetNum(m_nWidthCount * m_nHeightCount);

	static ConstructorHelpers::FObjectFinder<UTexture> HUDTextureOb(TEXT("/Game/HUD/Textures/InvDarkTransparent64"));
	m_texture = HUDTextureOb.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDTextureSelOb(TEXT("/Game/HUD/Textures/InvSel64"));
	m_textureSelected = HUDTextureSelOb.Object;

	m_nHoverIndex = -1;

	m_pInventory = NULL;
	m_pPreviousInventory = NULL;

	m_bInGroup = false;

	m_nDraggingItemIndex = -1;
}

void AInventory::Init(ACustomHUD * pHUD, AHotbar * pHotbar, float fSlotSize, float fInventoryBorder)
{
	m_pHUD = pHUD;
	m_pHotbar = pHotbar;
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

	m_pPreviousInventory = pPreviousInventory;

	OpenInventory(pInventory, bInGroup);
}

void AInventory::CloseInventory()
{
	m_bInvOpen = false;
}

void AInventory::OpenInventory(AInventoryObject* pInventory, bool bInGroup)
{
	if (pInventory == NULL)
	{
		return;
	}
		
	m_pInventory = pInventory;

	if (m_bInGroup != bInGroup || pInventory->m_nInvWidthCount != m_nWidthCount || pInventory->m_nInvHeightCount != m_nHeightCount)
	{
		// inventory group, width or height has changed
		m_nWidthCount = pInventory->m_nInvWidthCount;
		m_nHeightCount = pInventory->m_nInvHeightCount;
		m_bInGroup = bInGroup;

		m_vaInvHitBoxPositions.SetNum(m_nWidthCount * m_nHeightCount);
		UpdatePositions();
	}

	m_bInvOpen = true;
}

void AInventory::DrawInventory()
{
	if (!m_bInvOpen || m_vaInvHitBoxPositions.Num() != m_nWidthCount * m_nHeightCount)
	{
		return;
	}

	int i;
	for (i = 0; i < m_vaInvHitBoxPositions.Num(); i++)
	{
		// hitbox
		FString name = FString::FromInt(i);
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
		if (m_nDraggingItemIndex != i && m_pInventory->HasItem(i))
		{
			// TODO: review and scale of item texture should match without the hardcoded scaling....
			m_pHUD->DrawTextureSimple(m_pInventory->m_inventorySlots[i]->m_inventoryTexture, m_vaInvHitBoxPositions[i].X, m_vaInvHitBoxPositions[i].Y, 0.5f * m_pHUD->GetCurrentRatio());
		}
	}

	if (m_nDraggingItemIndex >= 0)
	{
		// draw texture of item being dragged under mouse cursor
		DrawDraggedItem();
	}
}

void AInventory::DrawDraggedItem()
{
	APlayerController* const controller = Cast<APlayerController>(m_pHUD->PlayerOwner);
	if (controller == NULL)
	{
		return;
	}

	float x, y;
	controller->GetMousePosition(x, y);

	m_pHUD->DrawTextureSimple(m_pInventory->m_inventorySlots[m_nDraggingItemIndex]->m_inventoryTexture, x, y, m_pHUD->GetCurrentRatio());
}

void AInventory::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}


void AInventory::SetStartPosition()
{
	if (m_pHUD == NULL)
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
		// single inventory opened - draw it above hotbar on right side (minus its width and margin)

		x = m_pHUD->VScreenDimensions.X - fInvWidth - fMargin;

		if (m_pHotbar && m_pHotbar->IsHotbarVisible())
		{
			// inventory position above hotbar
			y = m_pHotbar->GetStartPos().Y - m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nHeightCount;
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

bool AInventory::ItemDrag(bool bPickup)
{
	if (!bPickup)
	{
		// TODO: this needs to be changed to store it in CustomHUD and so check across all inventories and hotbar, but for now...
		if (m_nDraggingItemIndex >= 0)
		{			
			if (m_pInventory == NULL || m_nHoverIndex < 0 || m_nDraggingItemIndex == m_nHoverIndex)
			{
				// it was us... but do nothing except drop item being dragged
				m_nDraggingItemIndex = -1;
				return true;
			}

			// move to new index

			// TODO: this needs to replicate to/from server...
			// also the move will get trickier when not same inventory/hotbar, probably retrieve and then add
			m_pInventory->MoveItem(m_nDraggingItemIndex, m_nHoverIndex, true);
			

			// drop item being dragged
			m_nDraggingItemIndex = -1;
			return true;
		}
		
		// not us...
		return false;
	}

	if (!m_bInvOpen || m_nHoverIndex < 0)
	{
		return false;
	}

	// we have a hover index, so now we just need to check if slot has an item
	if (!m_pInventory->HasItem(m_nHoverIndex))
	{
		// no item (or potentially out-of-bounds)
		return false;
	}

	// start "dragging" item - which simply means tracking which item we were hovering over when drag started
	m_nDraggingItemIndex = m_nHoverIndex;

	return true;
}

bool AInventory::CheckMouseOver(const FName BoxName, bool bBegin)
{
	// TODO: note - each inventory (there will eventually be multiple) must have its own unique BoxName
	// TODO: easiest achieved by having the first part of BoxName indicate inventory
	// i.e. "I0", "I1", etc. - see Hotbar for how to implemnet

	if (!m_bInvOpen)
	{
		// inventory is not open so we know it can't be a hitbox in this inventory
		return false;
	}

	// TODO: with support for multiple inventories this needs to get smarter and check we are in right inventory first
	// it also needs to store one reference to the current hitbox for all inventories + hotbar in the CustomHUD

	FString strHitboxName = BoxName.ToString();
	if (!strHitboxName.IsNumeric())
	{
		// all inventory slots are named to their index so this cannot be an inventory slot
		return false;
	}

	// get inventory slot index number
	int nIndex = FCString::Atoi(*strHitboxName);
	
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

