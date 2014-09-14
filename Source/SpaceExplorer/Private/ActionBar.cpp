
#include "SpaceExplorer.h"
#include "ActionBar.h"
#include "CustomHUD.h"
#include "DragObject.h"

AActionBar::AActionBar(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	m_bShowActionBar = true;
	m_nSlotCount = 5;
	m_pHUD = NULL;

	m_fSlotSize = 128.f;

	m_actionSlots.SetNum(m_nSlotCount);

	// default HUD green
	m_textColour = FLinearColor(0.391f, 0.735f, 0.213f);

	// default HUD green
	m_textColourSelected = FLinearColor(0.512f, 1.0f, 0.085f);

	m_fTextScale = 1.f;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDActionBarTextureOb(TEXT("/Game/HUD/Textures/HotBarDarkTransparent128"));
	m_texture = HUDActionBarTextureOb.Object;

	static ConstructorHelpers::FObjectFinder<UTexture> HUDActionBarTextureSelOb(TEXT("/Game/HUD/Textures/HotbarButtonPressed128"));
	m_textureSelected = HUDActionBarTextureSelOb.Object;

	static ConstructorHelpers::FObjectFinder<UFont> HUDActionBarFontOb(TEXT("/Game/Fonts/FadeToGrey"));
	m_font = HUDActionBarFontOb.Object;

	m_nHoverIndex = -1;
}

void AActionBar::Init(ACustomHUD * pHUD, float fTextScale, float fSlotSize)
{
	m_pHUD = pHUD;
	m_fSlotSize = fSlotSize;
	m_fTextScale = fTextScale;
}

bool AActionBar::IsVisible()
{
	return m_bShowActionBar;
}

void AActionBar::Draw()
{
	if (!m_bShowActionBar || m_actionSlots.Num() != m_nSlotCount)
	{
		return;
	}

	int i;
	for (i = 0; i < m_nSlotCount; i++)
	{
		// hitbox
		FString name = TEXT("H") + FString::FromInt(i);
		m_pHUD->AddHitBox(m_actionSlots[i].m_hitBoxPosition, FVector2D(m_fSlotSize, m_fSlotSize) * m_pHUD->GetCurrentRatio(), FName(*name), false);

		UTexture *texture = m_texture;
		FLinearColor * textColour = &m_textColour;
		if (m_nHoverIndex == i)
		{
			// selected
			texture = m_textureSelected;
			textColour = &m_textColourSelected;
		}

		// texture
		if (texture)
		{
			m_pHUD->DrawTextureSimple(texture, m_actionSlots[i].m_hitBoxPosition.X, m_actionSlots[i].m_hitBoxPosition.Y, m_pHUD->GetCurrentRatio());
		}

		// font
		if (m_font)
		{
			m_pHUD->DrawText(FString::FromInt(i + 1), *textColour, m_actionSlots[i].m_hitBoxPosition.X + 5.0f, m_actionSlots[i].m_hitBoxPosition.Y, m_font, m_fTextScale);
		}
	}
}

void AActionBar::SetSlotCount(int32 nCount)
{
	m_nSlotCount = nCount;
	m_actionSlots.SetNum(m_nSlotCount);

	UpdatePositions();
}

void AActionBar::UpdatePositions()
{
	SetStartPosition();
	SetHitBoxPositionArray();
}

void AActionBar::SetStartPosition()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// size of entire AActionBar
	float fSize = m_fSlotSize * m_pHUD->GetCurrentRatio() * m_nSlotCount;

	// start (upper, left corner) of vertical action bar (multiply by 1.5 to get a bit of margin)
	m_vStartPos = FVector2D(m_pHUD->VScreenDimensions.X / 2.3f - fSize / 2, m_pHUD->VScreenDimensions.Y - m_fSlotSize * 1.5f * m_pHUD->GetCurrentRatio());
}

void AActionBar::SetHitBoxPositionArray()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	int i;
	for (i = 0; i < m_actionSlots.Num(); i++)
	{
		// use a fraction of the action bar size
		float fSpacing = m_fSlotSize * m_pHUD->GetCurrentRatio() / 10.0f * i;
		float X = (i + 1) * m_fSlotSize *  m_pHUD->GetCurrentRatio() + fSpacing + m_vStartPos.X;

		m_actionSlots[i].m_hitBoxPosition = FVector2D(X, m_vStartPos.Y);
	}
}

FVector2D AActionBar::GetStartPos()
{
	return m_vStartPos;
}

bool AActionBar::DragDrop(bool bPickup, const class DragObject& item)
{
	/*if (!bPickup)
	{
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
			// also the move will get trickier when not same inventory, probably retrieve and then add
			m_pInventory->MoveItem(m_nDraggingItemIndex, m_nHoverIndex, true);


			// drop item being dragged
			m_nDraggingItemIndex = -1;
			return true;
		}

		// not us...
		return false;
	}

	if (!m_bShowActionBar || m_nHoverIndex < 0)
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
	m_nDraggingItemIndex = m_nHoverIndex;*/

	return true;
}

bool AActionBar::CheckMouseOver(const FName BoxName, bool bBegin)
{
	if (!m_bShowActionBar)
	{
		return false;
	}

	if (bBegin)
	{
		m_cursorOverHitBoxName = BoxName;

		FString strHitboxName = m_cursorOverHitBoxName.ToString();
		if (strHitboxName.Left(1).Compare(TEXT("H")) == 0)
		{
			// action bar slot
			if (strHitboxName.Mid(1).IsNumeric())
			{
				m_nHoverIndex = FCString::Atoi(*strHitboxName.Mid(1));
				return true;
			}
		}

		return false;
	}

	m_nHoverIndex = -1;

	return false;
}

bool AActionBar::SetSlotAction(int32 nIndex, const class DragObject& object)
{
	if (nIndex < 0 || nIndex >= m_actionSlots.Num())
	{
		// invalid index
		return false;
	}

	m_actionSlots[nIndex].m_object = object;

	return true;
}

void AActionBar::DisableSlot(int32 nIndex)
{
	if (nIndex < 0 || nIndex >= m_actionSlots.Num())
	{
		// invalid index
		return;
	}

	m_actionSlots[nIndex].m_object.Drop();
}
