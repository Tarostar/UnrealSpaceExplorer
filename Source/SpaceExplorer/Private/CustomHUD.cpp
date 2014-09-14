

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "CustomHUD.h"
#include "SpaceExplorerPawn.h"
#include "ActionBar.h"
#include "Inventory.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

/* Init */

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bMenuOpen = false;

	m_fCurrentRatio = 1.0f;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	m_actionBar = NULL;

	
}

void ACustomHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	/* Menu */
	m_menu.Init(this);
}

void ACustomHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		

		/* ActionBar */
		m_actionBar = World->SpawnActor<AActionBar>(SpawnParams);
		if (m_actionBar)
		{	
			/*AInventoryObject* pHotbarObjects = NULL;
			APlayerController* const controller = Cast<APlayerController>(PlayerOwner);
			if (controller)
			{
				ASpaceExplorerPawn* const pawn = Cast<ASpaceExplorerPawn>(controller->GetPawn());
				if (pawn)
				{
					pHotbarObjects = pawn->GetHotbarObjects();
				}
			}*/

			m_actionBar->Init(this, 0.3f, 128.f);
		}
	}

}

/* HUD and Menu Draw */

void ACustomHUD::ReceiveDrawHUD(int32 SizeX, int32 SizeY)
{
	Super::ReceiveDrawHUD(SizeX, SizeY);

	OnReceiveDrawHUD(SizeX, SizeY);
}

void ACustomHUD::OnReceiveDrawHUD(int32 SizeX, int32 SizeY)
{
	// update screen dimensions if they change beyond tolerance specified in IsNearlyEqual
	if (!FMath::IsNearlyEqual(VScreenDimensions.X, SizeX, 10.f) || !FMath::IsNearlyEqual(VScreenDimensions.Y, SizeY, 10.f))
	{
		UpdateScreenDimensions(SizeX, SizeY);
	}
	
	if (m_bMenuOpen)
	{
		// menu open - draw main menu
		m_menu.DrawMainMenu();

		// allow blueprint chance to do any menu specific work
		MenuDrawCompleted();
		return;
	}

	// draw HUD and inventory
	DrawHUDComponents();	

	// allow blueprint chance to do any HUD specific work
	HUDDrawCompleted();
}

void ACustomHUD::UpdateScreenDimensions(int32 SizeX, int32 SizeY)
{
	// this calss should only be called when screen size has changed

	// set class FVector2D that holds current screen size
	VScreenDimensions.Set(SizeX, SizeY);

	// set class float with CurrentRatio use to scale HUD and menu
	m_fCurrentRatio = ScaleToScreensize();

	// update inventory hitboxes, etc.
	if (m_actionBar)
	{
		m_actionBar->UpdatePositions();
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			m_inventories[i]->UpdatePositions();
		}
	}
}

void ACustomHUD::DrawHUDComponents()
{
	// TODO...

	//DrawHUDBars();

	if (m_actionBar)
	{
		m_actionBar->Draw();
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			m_inventories[i]->DrawInventory();
		}
	}

	DrawDraggedItem();
}

/* Helpers */

float ACustomHUD::ScaleToScreensize()
{
	// set ratio from width
	float ratio = VScreenDimensions.X / DEFAULT_SCREEN_WIDTH;

	// if height is a smaller ratio use that instead
	if (ratio > VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT)
	{
		ratio = VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT;
	}

	return ratio;
}

float ACustomHUD::ScaleToScreensize(float& OutSizeX, float& OutSizeY)
{
	float ratio = ScaleToScreensize();

	// scale X and Y to ratio
	OutSizeX *= ratio;
	OutSizeY *= ratio;

	return ratio;
}

/* Menu Events */

void ACustomHUD::ReceiveHitBoxClick(const FName BoxName)
{
	Super::ReceiveHitBoxClick(BoxName);

	if (m_menu.ReceiveHitBox(BoxName, true))
		return;

	if (m_actionBar)
	{
		if (m_actionBar->DragDrop(true, m_draggedItem))
		{
			return;
		}
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			if (m_inventories[i]->ItemDrag(true, m_draggedItem))
			{
				return;
			}
		}
	}
}

void ACustomHUD::ReceiveHitBoxRelease(const FName BoxName)
{
	Super::ReceiveHitBoxRelease(BoxName);

	if (m_menu.ReceiveHitBox(BoxName, false))
		return;
	
	if (m_actionBar)
	{
		if (m_actionBar->DragDrop(false, m_draggedItem))
		{
			return;
		}
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			if (m_inventories[i]->ItemDrag(false, m_draggedItem))
			{
				return;
			}
		}
	}

	// release back into world
	// TODO: instatiate - or actually make component visible again
	m_draggedItem.Drop();
}


void ACustomHUD::ReceiveHitBoxBeginCursorOver(const FName BoxName)
{

	Super::ReceiveHitBoxBeginCursorOver(BoxName);
	
	if (m_actionBar)
	{
		if (m_actionBar->CheckMouseOver(BoxName, true))
		{
			return;
		}
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			if (m_inventories[i]->CheckMouseOver(BoxName, true))
			{
				return;
			}
		}
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonHover);
}

void ACustomHUD::ReceiveHitBoxEndCursorOver(const FName BoxName)
{
	Super::ReceiveHitBoxEndCursorOver(BoxName);
	
	if (m_actionBar)
	{
		if (m_actionBar->CheckMouseOver(BoxName, false))
		{
			return;
		}
	}

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i])
		{
			if (m_inventories[i]->CheckMouseOver(BoxName, false))
			{
				return;
			}
		}
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonNormal);
}

float ACustomHUD::GetCurrentRatio()
{
	return m_fCurrentRatio;
}

void ACustomHUD::AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size)
{
	m_menu.AutoGenerateMainMenu(location, textureNormal, textureHover, texturePressed, font, size);
}

void ACustomHUD::ToggleAllInventory()
{
	// open or close all inventories

	if (m_inventories.Num() > 0)
	{
		// close all open inventories

		for (int i = 0; i < m_inventories.Num(); i++)
		{
			// TODO: this is a bit manual and clunky
			m_inventories[i]->CloseInventory();
		}

		m_inventories.Empty();

		// all closed
		return;
	}

	// none open, so open all

	UWorld* const World = GetWorld();
	if (World == NULL)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
	
	ASpaceExplorerPawn* pPawn = GetSpaceExplorerPawn();
	if (pPawn)
	{
		m_inventories.SetNum(pPawn->GetInventoryObjectCount());

		for (int i = 0; i < m_inventories.Num(); i++)
		{
			m_inventories[i] = World->SpawnActor<AInventory>(SpawnParams);
			if (m_inventories[i])
			{
				m_inventories[i]->Init(this, m_actionBar);
				m_inventories[i]->OpenInventory(pPawn->GetInventoryObjectFromIndex(i), m_inventories.Num() > 1, i == 0 ? NULL : m_inventories[i - 1]);
			}
		}
	}
}

void ACustomHUD::ToggleInventory(int32 nID)
{
	// open or close indicated inventory

	for (int i = 0; i < m_inventories.Num(); i++)
	{
		if (m_inventories[i]->GetID() == nID)
		{
			// found - close
			m_inventories[i]->CloseInventory();
			m_inventories.RemoveAt(i);
			return;
		}
	}

	// did not find - open

	UWorld* const World = GetWorld();
	if (World == NULL)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;

	ASpaceExplorerPawn* pPawn = GetSpaceExplorerPawn();
	if (pPawn)
	{
		AInventory* newInventory = World->SpawnActor<AInventory>(SpawnParams);
		if (newInventory == NULL)
		{
			// failed...
			return;
		}
		
		newInventory->Init(this, m_actionBar);

		// handle positioning if other inventories already open
		bool bExisting = m_inventories.Num() > 0;
		newInventory->OpenInventory(pPawn->GetInventoryObjectFromID(nID), bExisting, bExisting ? m_inventories[m_inventories.Num() - 1] : NULL);

		// add to array of open inventories
		m_inventories.Add(newInventory);
	}
}

void ACustomHUD::DrawDraggedItem()
{
	if (!m_draggedItem.IsDragging())
	{
		return;
	}

	APlayerController* const controller = Cast<APlayerController>(PlayerOwner);
	if (controller == NULL)
	{
		return;
	}

	float x, y;
	controller->GetMousePosition(x, y);

	AInventoryObject* const pInventory = GetSourceInventoryObjectFromID(m_draggedItem.GetInventoryID());		
	if (pInventory)
	{
		DrawTextureSimple(pInventory->m_inventorySlots[m_draggedItem.GetSlotIndex()]->m_inventoryTexture, x, y, GetCurrentRatio());
	}
}

const class DragObject* ACustomHUD::GetDraggedItem()
{
	return &m_draggedItem;
}

bool ACustomHUD::IsDragging()
{
	return m_draggedItem.IsDragging();
}

FString ACustomHUD::GetLabel()
{
	return m_draggedItem.GetLabel();
}

EActionType::Type ACustomHUD::GetType()
{
	return m_draggedItem.GetType();
}

int32 ACustomHUD::GetSlotIndex()
{
	return m_draggedItem.GetSlotIndex();
}

int32 ACustomHUD::GetInventoryID()
{
	return m_draggedItem.GetInventoryID();
}

AInventoryObject* ACustomHUD::GetSourceInventoryObjectFromID(int32 nID)
{
	ASpaceExplorerPawn* const pPawn = GetSpaceExplorerPawn();
	if (pPawn)
	{
		return pPawn->GetInventoryObjectFromID(m_draggedItem.GetInventoryID());
	}

	return NULL;
}

ASpaceExplorerPawn* ACustomHUD::GetSpaceExplorerPawn()
{
	APlayerController* const controller = Cast<APlayerController>(PlayerOwner);
	if (controller == NULL)
	{
		return NULL;
	}

	return Cast<ASpaceExplorerPawn>(controller->GetPawn());
}
