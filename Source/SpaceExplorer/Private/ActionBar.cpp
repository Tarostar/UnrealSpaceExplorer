
#include "SpaceExplorer.h"
#include "ActionBar.h"
#include "CustomHUD.h"
#include "DragObject.h"
#include "InventoryObject.h"

AActionBar::AActionBar(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	m_bShowActionBar = true;
	m_pHUD = NULL;

	m_fSlotSize = 128.f;

	m_actionSlots.SetNum(5);

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
	if (!m_bShowActionBar)
	{
		return;
	}

	int i;
	for (i = 0; i < m_actionSlots.Num(); i++)
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

		if (m_actionSlots[i].m_object.GetInventoryID() >= 0)
		{
			// first retrieve it
			AInventoryObject* const pSourceInventory = m_pHUD->GetSourceInventoryObjectFromID(m_actionSlots[i].m_object.GetInventoryID());
			if (pSourceInventory)
			{

				AUsableObject * pItem = pSourceInventory->GetItem(m_actionSlots[i].m_object.GetSlotIndex());
				if (pItem)
				{
					m_pHUD->DrawTextureSimple(pItem->m_inventoryTexture, m_actionSlots[i].m_hitBoxPosition.X, m_actionSlots[i].m_hitBoxPosition.Y, m_pHUD->GetCurrentRatio());
				}
			}
		}
		
	}
}

void AActionBar::SetSlotCount(int32 nCount)
{
	m_actionSlots.SetNum(nCount);

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
	float fSize = m_fSlotSize * m_pHUD->GetCurrentRatio() * m_actionSlots.Num();

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

bool AActionBar::DragDrop(bool bPickup, class DragObject& item)
{
	if (!bPickup)
	{
		if (item.IsDragging())
		{	
			// item is being dragged

			if (m_nHoverIndex < 0)
			{
				// item is not being dropped into this action bar
				return false;
			}

			// item being dragged from an inventory

			// TODO: does this need to replicate to/from server...

			// add to action bar
			SetSlotAction(m_nHoverIndex, item);			
			item.Drop();
			return true;
		}
		
		// nothing to do...
		return false;
	}

	if (!m_bShowActionBar || m_nHoverIndex < 0)
	{
		// not this action bar
		return false;
	}

	// set dragged item to this one
	item = m_actionSlots[m_nHoverIndex].m_object;

	// remove action from slot
	DisableSlot(m_nHoverIndex);

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

bool AActionBar::SetSlotAction(int32 nIndex, class DragObject& object)
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

bool AActionBar::InvokeAction()
{
	if (!m_bShowActionBar || m_nHoverIndex < 0)
	{
		// not this action bar
		return false;
	}

	// set dragged item to this one
	m_actionSlots[m_nHoverIndex].m_object.InvokeAction();

	return true;
}
