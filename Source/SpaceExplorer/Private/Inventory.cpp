

#include "SpaceExplorer.h"
#include "CustomHUD.h"
#include "Inventory.h"

Inventory::Inventory()
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
}

Inventory::~Inventory()
{
}

void Inventory::Init(ACustomHUD * pHUD, Hotbar * pHotbar, float fSlotSize, float fInventoryBorder)
{
	m_pHUD = pHUD;
	m_pHotbar = pHotbar;
	m_fSlotSize = fSlotSize;
	m_fInventoryBorder = fInventoryBorder;
}

bool Inventory::IsInvOpen()
{
	return m_bInvOpen;
}

void Inventory::ToggleInventory(int32 nWidthCount, int32 nHeightCount)
{
	if (m_bInvOpen)
	{
		m_bInvOpen = false;
		return;
	}

	OpenInventory(nWidthCount, nHeightCount);
}

void Inventory::OpenInventory(int32 nWidthCount, int32 nHeightCount)
{
	if (nWidthCount != m_nWidthCount || nHeightCount != m_nHeightCount)
	{
		// inventory width or height has changed
		m_nWidthCount = nWidthCount;
		m_nHeightCount = nHeightCount;

		m_vaInvHitBoxPositions.SetNum(m_nWidthCount * m_nHeightCount);
		UpdatePositions();
	}

	m_bInvOpen = true;
}

void Inventory::DrawInventory()
{
	if (!m_bInvOpen)
	{
		return;
	}
}

void Inventory::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}


void Inventory::SetStartPosition()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// inventory width
	float fInvWidth = m_fSlotSize * m_pHUD->CurrentRatio * m_nWidthCount;

	// inventory positioned to the right (minus its width and margin)
	float fMargin = m_fSlotSize;
	float x = m_pHUD->VScreenDimensions.X - fInvWidth - fMargin;

	float y;
	if (m_pHotbar)
	{
		// inventory position above hotbar
		y = m_pHotbar->GetStartPos().Y - m_fSlotSize * m_pHUD->CurrentRatio * m_nHeightCount;
	}
	else
	{
		// inventory position above bottom of screen
		y = m_pHUD->VScreenDimensions.Y - m_fSlotSize * m_pHUD->CurrentRatio * m_nHeightCount;
	}

	m_vInvStartpos = FVector2D(x, y);
}

void Inventory::SetHitBoxPositionArray()
{
	int i;
	for (i = 0; i < m_vaInvHitBoxPositions.Num(); i++)
	{
		int32 nColumn = i / m_nHeightCount;
		int32 nRow = i - nColumn * m_nWidthCount;

		float X = nRow * m_fSlotSize * m_pHUD->CurrentRatio + m_vInvStartpos.X + m_fInventoryBorder * m_pHUD->CurrentRatio;
		float Y = nColumn * m_fSlotSize * m_pHUD->CurrentRatio + m_vInvStartpos.Y + m_fInventoryBorder * m_pHUD->CurrentRatio;

		m_vaInvHitBoxPositions[i] = FVector2D(X, Y);
	}
}

void Inventory::ItemDrag(bool bPickup)
{

}

