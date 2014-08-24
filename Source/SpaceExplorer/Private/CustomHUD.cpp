

#include "SpaceExplorer.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

/* Init */

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_fCurrentRatio = 1.0f;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
}

void ACustomHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	/* Hotbar */
	static ConstructorHelpers::FObjectFinder<UFont> HUDHotbarFontOb(TEXT("/Engine/Fonts/LED36"));
	static ConstructorHelpers::FObjectFinder<UTexture> HUDHotbarTextureOb(TEXT("/Engine/HUD/Textures/HotBarButton128"));

	m_hotbar = World->SpawnActor<AHotbar>(AHotbar::StaticClass());
	m_hotbar->Init(this, HUDHotbarTextureOb.Object, FColor::White, HUDHotbarFontOb.Object, 1.f, 128.f);

	/* Menu */
	m_menu.Init(this);

	/* Inventory */
	m_inventory = World->SpawnActor<AInventory>(AInventory::StaticClass());
	m_inventory->Init(this, m_hotbar);
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
	
	if (m_menu.IsMenuOpen())
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
	m_hotbar->UpdatePositions();

	m_inventory->UpdatePositions();
}

void ACustomHUD::DrawHUDComponents()
{
	// TODO...

	//DrawHUDBars();

	m_hotbar->DrawHotbar();

	m_inventory->DrawInventory();
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

	if (m_inventory->IsInvOpen())
	{
		// in item mode, check for item pickup
		m_inventory->ItemDrag(true);
	}
}

void ACustomHUD::ReceiveHitBoxRelease(const FName BoxName)
{
	Super::ReceiveHitBoxRelease(BoxName);

	if (m_menu.ReceiveHitBox(BoxName, false))
		return;

	if (m_inventory->IsInvOpen())
	{
		// in item mode, check for item pickup
		m_inventory->ItemDrag(false);
	}
}


void ACustomHUD::ReceiveHitBoxBeginCursorOver(const FName BoxName)
{
	Super::ReceiveHitBoxBeginCursorOver(BoxName);

	if (m_inventory->IsInvOpen())
	{
		CursorOverHitBoxName = BoxName;
		bCursorOverHitBox = true;
		return;
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonHover);
}

void ACustomHUD::ReceiveHitBoxEndCursorOver(const FName BoxName)
{
	Super::ReceiveHitBoxEndCursorOver(BoxName);

	if (m_inventory->IsInvOpen())
	{
		bCursorOverHitBox = false;
		return;
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonNormal);
}

float ACustomHUD::GetCurrentRatio()
{
	return m_fCurrentRatio;
}

