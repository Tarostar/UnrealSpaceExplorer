

#include "SpaceExplorer.h"
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

	m_bCursorOverHitBox = false;
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

void AInventory::ToggleInventory(int32 nWidthCount, int32 nHeightCount)
{
	if (m_bInvOpen)
	{
		m_bInvOpen = false;
		return;
	}

	OpenInventory(nWidthCount, nHeightCount);
}

void AInventory::OpenInventory(int32 nWidthCount, int32 nHeightCount)
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

void AInventory::DrawInventory()
{
	if (!m_bInvOpen)
	{
		return;
	}
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
	float fInvWidth = m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nWidthCount;

	// inventory positioned to the right (minus its width and margin)
	float fMargin = m_fSlotSize;
	float x = m_pHUD->VScreenDimensions.X - fInvWidth - fMargin;

	float y;
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

	m_vInvStartpos = FVector2D(x, y);
}

void AInventory::SetHitBoxPositionArray()
{
	int i;
	for (i = 0; i < m_vaInvHitBoxPositions.Num(); i++)
	{
		int32 nColumn = i / m_nHeightCount;
		int32 nRow = i - nColumn * m_nWidthCount;

		float X = nRow * m_fSlotSize * m_pHUD->GetCurrentRatio() + m_vInvStartpos.X + m_fInventoryBorder * m_pHUD->GetCurrentRatio();
		float Y = nColumn * m_fSlotSize * m_pHUD->GetCurrentRatio() + m_vInvStartpos.Y + m_fInventoryBorder * m_pHUD->GetCurrentRatio();

		m_vaInvHitBoxPositions[i] = FVector2D(X, Y);
	}
}

bool AInventory::ItemDrag(bool bPickup)
{
	// TODO: implement

	return false;
}

bool AInventory::CheckMouseOver(const FName BoxName, bool bBegin)
{
	// TODO: implement
	if (bBegin)
	{
		m_cursorOverHitBoxName = BoxName;
		m_bCursorOverHitBox = true;
		return false;
	}

	m_bCursorOverHitBox = false;

	return false;
}

