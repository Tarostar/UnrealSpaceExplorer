

#include "SpaceExplorer.h"
#include "CustomHUD.h"
#include "InventoryObject.h"
#include "Hotbar.h"

AHotbar::AHotbar(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bShowHotbar = true;
	m_nSlotCount = 5;
	m_pHUD = NULL;

	m_fHotbarSlotSize = 128.f;

	m_vaHotbarHitBoxPositions.SetNum(m_nSlotCount);

	// default HUD green
	m_textColour = FLinearColor(0.391f, 0.735f, 0.213f);

	// default HUD green
	m_textColourSelected = FLinearColor(0.512f, 1.0f, 0.085f);

	m_fTextScale = 1.f;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDHotbarTextureOb(TEXT("/Game/HUD/Textures/HotBarDarkTransparent128"));
	m_texture = HUDHotbarTextureOb.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDHotbarTextureSelOb(TEXT("/Game/HUD/Textures/HotbarButtonPressed128"));
	m_textureSelected = HUDHotbarTextureSelOb.Object;

	static ConstructorHelpers::FObjectFinder<UFont> HUDHotbarFontOb(TEXT("/Game/Fonts/FadeToGrey"));
	m_font = HUDHotbarFontOb.Object;

	m_nHotbarHoverIndex = -1;

	m_pHotbarObjects = NULL;
}

void AHotbar::Init(ACustomHUD * pHUD, float fTextScale, float fSlotSize, AInventoryObject* pHotbarObjects)
{
	m_pHUD = pHUD;
	m_fHotbarSlotSize = fSlotSize;
	m_fTextScale = fTextScale;
	m_pHotbarObjects = pHotbarObjects;
}

bool AHotbar::IsHotbarVisible()
{
	return m_bShowHotbar;
}

void AHotbar::DrawHotbar()
{
	if (!m_bShowHotbar || m_vaHotbarHitBoxPositions.Num() != m_nSlotCount)
	{
		return;
	}

	int i;
	for (i = 0; i < m_nSlotCount; i++)
	{
		// hitbox
		FString name = TEXT("H") + FString::FromInt(i);
		m_pHUD->AddHitBox(m_vaHotbarHitBoxPositions[i], FVector2D(m_fHotbarSlotSize, m_fHotbarSlotSize) * m_pHUD->GetCurrentRatio(), FName(*name), false);

		UTexture *texture = m_texture;
		FLinearColor * textColour = &m_textColour;
		if (m_nHotbarHoverIndex == i)
		{
			// selected
			texture = m_textureSelected;
			textColour = &m_textColourSelected;
		}

		// texture
		if (texture)
		{
			m_pHUD->DrawTextureSimple(texture, m_vaHotbarHitBoxPositions[i].X, m_vaHotbarHitBoxPositions[i].Y, m_pHUD->GetCurrentRatio());
		}
		
		// font
		if (m_font)
		{
			m_pHUD->DrawText(FString::FromInt(i + 1), *textColour, m_vaHotbarHitBoxPositions[i].X + 5.0f, m_vaHotbarHitBoxPositions[i].Y, m_font, m_fTextScale);
		}
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

	// start (upper, left corner) of vertical hotbar (multiply by 1.5 to get a bit of margin)
	m_vHotbarStartPos = FVector2D(m_pHUD->VScreenDimensions.X / 2.3f - fHotbarSize / 2, m_pHUD->VScreenDimensions.Y - m_fHotbarSlotSize * 1.5f * m_pHUD->GetCurrentRatio());
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

bool AHotbar::ItemDrag(bool bPickup)
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

	if (!m_bShowHotbar || m_nHoverIndex < 0)
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

bool AHotbar::CheckMouseOver(const FName BoxName, bool bBegin)
{
	if (!m_bShowHotbar)
	{
		return false;
	}

	// TODO: implement
	if (bBegin)
	{
		m_cursorOverHitBoxName = BoxName;

		FString strHitboxName = m_cursorOverHitBoxName.ToString();
		if (strHitboxName.Left(1).Compare(TEXT("H")) == 0)
		{
			// hotbar button
			if (strHitboxName.Right(1).IsNumeric())
			{
				m_nHotbarHoverIndex = FCString::Atoi(*strHitboxName.Right(1));
				return true;
			}
		}	
		
		return false;
	}
	
	m_nHotbarHoverIndex = -1;

	return false;
}

void AHotbar::LinkHotbar(AInventoryObject* pHotbarObjects)
{
	m_pHotbarObjects = pHotbarObjects;
}
