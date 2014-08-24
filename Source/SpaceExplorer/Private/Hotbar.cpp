

#include "SpaceExplorer.h"
#include "CustomHUD.h"
#include "Hotbar.h"

AHotbar::AHotbar(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bShowHotbar = false;
	m_nSlotCount = 5;
	m_pHUD = NULL;

	m_fHotbarSlotSize = 128.f;

	m_vaHotbarHitBoxPositions.SetNum(m_nSlotCount);

	// TODO: review
	m_textColour = FColor::White;

	m_fTextScale = 0.3f;

	m_font = NULL;
	m_texture = NULL;
}

void AHotbar::Init(ACustomHUD * pHUD, UTexture *texture, const FLinearColor& textColour, UFont* font, float fTextScale, float fSlotSize)
{
	m_pHUD = pHUD;
	m_texture = texture;
	m_fHotbarSlotSize = fSlotSize;
	m_textColour = textColour;
	m_font = font;
	m_fTextScale = fTextScale;
}

bool AHotbar::IsHotbarVisible()
{
	return m_bShowHotbar;
}

void AHotbar::DrawHotbar()
{
	if (m_vaHotbarHitBoxPositions.Num() != m_nSlotCount)
	{
		return;
	}

	int i;
	for (i = 0; i < m_nSlotCount; i++)
	{
		m_pHUD->DrawTextureSimple(m_texture, m_vaHotbarHitBoxPositions[i].X, m_vaHotbarHitBoxPositions[i].Y, m_pHUD->GetCurrentRatio());

		m_pHUD->DrawText(FString::FromInt(i), m_textColour, m_vaHotbarHitBoxPositions[i].X + 5.0f, m_vaHotbarHitBoxPositions[i].Y, m_font, m_fTextScale);
	}
}

void AHotbar::SetSlotCount(int32 nCount)
{
	m_nSlotCount = nCount;
	m_vaHotbarHitBoxPositions.SetNum(m_nSlotCount);

	UpdatePositions();
}

void AHotbar::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}

void AHotbar::SetStartPosition()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// size of entire AHotbar
	float fHotbarSize = m_fHotbarSlotSize * m_pHUD->GetCurrentRatio() * m_nSlotCount;

	// start (upper, left corner) of vertical hotbar
	m_vHotbarStartPos = FVector2D(m_pHUD->VScreenDimensions.X / 2.0f - fHotbarSize / 2, m_pHUD->VScreenDimensions.Y - m_fHotbarSlotSize * m_pHUD->GetCurrentRatio());
}

void AHotbar::SetHitBoxPositionArray()
{
	if (m_pHUD == NULL)
	{
		return;
	}
	
	int i;
	for (i = 0; i < m_vaHotbarHitBoxPositions.Num(); i++)
	{
		// use a fraction of the hotbar size
		float fSpacing = m_fHotbarSlotSize * m_pHUD->GetCurrentRatio() / 10.0f * i;
		float X = (i + 1) * m_fHotbarSlotSize *  m_pHUD->GetCurrentRatio() + fSpacing + m_vHotbarStartPos.X;

		m_vaHotbarHitBoxPositions[i] = FVector2D(X, m_vHotbarStartPos.Y);
	}
}

FVector2D AHotbar::GetStartPos()
{
	return m_vHotbarStartPos;
}
