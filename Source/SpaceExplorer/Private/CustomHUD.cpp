

#include "SpaceExplorer.h"
#include "InventoryObject.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

/* Init */

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_bMenuOpen = false;

	m_fCurrentRatio = 1.0f;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	m_hotbar = NULL;
	m_inventory = NULL;

	
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

		/* Hotbar */
		m_hotbar = World->SpawnActor<AHotbar>(SpawnParams);
		if (m_hotbar)
		{	
			m_hotbar->Init(this, 0.3f, 128.f);
		}

		/* Inventory */
		m_inventory = World->SpawnActor<AInventory>(SpawnParams);
		if (m_inventory)
		{
			m_inventory->Init(this, m_hotbar);
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
	if (m_hotbar)
	{
		m_hotbar->UpdatePositions();
	}

	if (m_inventory)
	{
		m_inventory->UpdatePositions();
	}
}

void ACustomHUD::DrawHUDComponents()
{
	// TODO...

	//DrawHUDBars();

	if (m_hotbar)
	{
		m_hotbar->DrawHotbar();
	}

	if (m_inventory)
	{
		m_inventory->DrawInventory();
	}
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

	if (m_inventory)
	{
		// in item mode, check for item pickup
		if (m_inventory->ItemDrag(true))
		{
			return;
		}
	}

	if (m_hotbar)
	{
		// in item mode, check for item pickup
		if (m_hotbar->ItemDrag(true))
		{
			return;
		}
	}
}

void ACustomHUD::ReceiveHitBoxRelease(const FName BoxName)
{
	Super::ReceiveHitBoxRelease(BoxName);

	if (m_menu.ReceiveHitBox(BoxName, false))
		return;

	if (m_inventory)
	{
		// in item mode, check for item pickup
		if (m_inventory->ItemDrag(false))
		{
			return;
		}
	}

	if (m_hotbar)
	{
		// in item mode, check for item pickup
		if (m_hotbar->ItemDrag(true))
		{
			return;
		}
	}
}


void ACustomHUD::ReceiveHitBoxBeginCursorOver(const FName BoxName)
{

	Super::ReceiveHitBoxBeginCursorOver(BoxName);
	
	if (m_inventory)
	{
		if (m_inventory->CheckMouseOver(BoxName, true))
		{
			return;
		}
	}

	if (m_hotbar)
	{
		if (m_hotbar->CheckMouseOver(BoxName, true))
		{
			return;
		}
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonHover);
}

void ACustomHUD::ReceiveHitBoxEndCursorOver(const FName BoxName)
{
	Super::ReceiveHitBoxEndCursorOver(BoxName);

	if (m_inventory)
	{
		if (m_inventory->CheckMouseOver(BoxName, false))
		{
			return;
		}
	}

	if (m_hotbar)
	{
		if (m_hotbar->CheckMouseOver(BoxName, false))
		{
			return;
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

void ACustomHUD::ToggleInventory(AInventoryObject* pInventory, bool bInGroup)
{
	if (m_inventory == NULL)
	{
		return;
	}

	// TODO: must trackk all inventories... (group class?)

	m_inventory->ToggleInventory(pInventory, bInGroup, NULL);

	// need to set start positions based on index, so decide on placement order
	// then need to handle UpdatePositions in the inventory
	// SetStartPosition

	// IDEA: pass in index, width, heigt, along with top, bottom and right of previous drawn inventory
	// then simple check to see if width fits starting at top of previous, otherwise start at bottom with horizontal left value of zero

	// issues: tracking open/closed individual inventories, allow positioning?

	// note top, bottom, right passed in as reference so that updated by each call!
}
