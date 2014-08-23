

#include "SpaceExplorer.h"
#include "CustomHUD.h"
#include "Hotbar.h"

Hotbar::Hotbar()
{
	m_bShowHotbar = false;
	m_nHotbarCount = 5;
	m_pHUD = NULL;

	m_fHotbarSlotSize = 128.f;

	m_vaHotbarHitBoxPositions.SetNum(m_nHotbarCount);
}

Hotbar::~Hotbar()
{
}

void Hotbar::Init(ACustomHUD * pHUD)
{
	m_pHUD = pHUD;
}

bool Hotbar::ShowHotbar()
{
	return m_bShowHotbar;
}

void Hotbar::DrawHotbar()
{

}

void Hotbar::SetHotbarCount(int32 nCount)
{
	m_nHotbarCount = nCount;
	m_vaHotbarHitBoxPositions.SetNum(m_nHotbarCount);

	UpdatePositions();
}

void Hotbar::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}

void Hotbar::SetStartPosition()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// size of entire hotbar
	float fHotbarSize = m_fHotbarSlotSize * m_pHUD->CurrentRatio * m_nHotbarCount;

	// start (upper, left corner) of vertical hotbar
	m_vHotbarStartPos = FVector2D(m_pHUD->VScreenDimensions.X / 2.0f - fHotbarSize / 2, m_pHUD->VScreenDimensions.Y - m_fHotbarSlotSize * m_pHUD->CurrentRatio);
}

void Hotbar::SetHitBoxPositionArray()
{
	if (m_pHUD == NULL)
	{
		return;
	}
	
	int i;
	for (i = 0; i < m_vaHotbarHitBoxPositions.Num(); i++)
	{
		// use a fraction of the hotbar size
		float fSpacing = m_fHotbarSlotSize * m_pHUD->CurrentRatio / 10.0f * i;
		float X = (i + 1) * m_fHotbarSlotSize *  m_pHUD->CurrentRatio + fSpacing + m_vHotbarStartPos.X;

		m_vaHotbarHitBoxPositions[i] = FVector2D(X, m_vHotbarStartPos.Y);
	}
}

FVector2D Hotbar::GetStartPos()
{
	return m_vHotbarStartPos;
}
