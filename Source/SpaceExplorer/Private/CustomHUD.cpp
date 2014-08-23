

#include "SpaceExplorer.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

/* Init */

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	CurrentRatio = 1.0f;
	bInventoryMode = false;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
}

void ACustomHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	m_menu.Init(this);
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
	CurrentRatio = ScaleToScreensize();

	// update inventory hitboxes, etc.
	SetInventoryPositions();
}

void ACustomHUD::DrawHUDComponents()
{
	// TODO...

	//DrawHUDBars();

	//DrawInventory();
}

/* Inventory - TODO: implement as its own class*/

void ACustomHUD::SetInventoryPositions()
{
	// TODO...

	SetHotbarStartPosition();

	SetInventoryStartPosition();

	SetHitBoxPositionArray();
}

void ACustomHUD::SetHotbarStartPosition()
{
	FVector2D HotbarStartPos;

	// number of slots (i.e. inventory boxes) in the hotbar
	int32 HotbarCount = 5;

	// size of one hotbar slot
	float HotbarSlotSize = 64.f;

	// size of entire hotbar
	float HotbarSize = HotbarSlotSize * CurrentRatio * HotbarCount;

	// start (upper, left corner) of vertical hotbar
	HotbarStartPos = FVector2D(VScreenDimensions.X / 2.0f - HotbarSize / 2, VScreenDimensions.Y - HotbarSlotSize * CurrentRatio);
}

void ACustomHUD::SetInventoryStartPosition()
{
/*	FVector2D InvStartPos;
	float InvSlotSize;
	int32 InvWidthCount = 10;
	int32 InvHeightCount = 5;

	// inventory width
	float InvWidth = InvSlotSize * CurrentRatio * InvWidthCount;

	// inventory positioned to the right (minus its width and margin)
	float Margin = InvSlotSize;
	float X = VScreenDimensions.X - InvWidth - Margin;

	// inventory position above hotbar
	float Y = HotbarStartPos.Y - InvSlotSize * CurrentRatio * InvHeightCount;

	InvStartPos = FVector2D(X, Y);*/
}

void ACustomHUD::SetHitBoxPositionArray()
{
	/*TArray<FVector2D> InvHitBoxPositions;
	TArray<FVector2D> HotbarHitBoxPositions;

	int i;
	for (i = 0; i < InvHitBoxPositions.Num(); i++)
	{
		int32 Column = i / InvHeightCount;
		int32 Row = i - Column * InvWidthCount;

		float X = Row * InvSlotSize * CurrentRatio + InvStartPos.X + InventoryBorder * CurrentRatio;
		float Y = Column * InvSlotSize * CurrentRatio + InvStartPos.Y + InventoryBorder * CurrentRatio;

		InvHitBoxPositions[i] = FVector2D(X, Y);
	}

	for (i = 0; i < HotbarHitBoxPositions.Num(); i++)
	{
		// use a fraction of the hotbar size
		float Spacing = HotbarSlotSize * CurrentRatio / 10.0f * i;
		float X = (i + 1) * HotbarSlotSize * CurrentRatio + Spacing + HotbarStartPos.X;

		HotbarHitBoxPositions[i] = FVector2D(X, HotbarStartPos.Y);
	}
	*/
}

void ACustomHUD::ItemDrag(bool bPickup)
{

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

	if (bInventoryMode)
	{
		// in item mode, check for item pickup
		ItemDrag(true);
	}
}

void ACustomHUD::ReceiveHitBoxRelease(const FName BoxName)
{
	Super::ReceiveHitBoxRelease(BoxName);

	if (m_menu.ReceiveHitBox(BoxName, false))
		return;

	if (bInventoryMode)
	{
		// in item mode, check for item pickup
		ItemDrag(false);
	}
}


void ACustomHUD::ReceiveHitBoxBeginCursorOver(const FName BoxName)
{
	Super::ReceiveHitBoxBeginCursorOver(BoxName);

	if (bInventoryMode)
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

	if (bInventoryMode)
	{
		bCursorOverHitBox = false;
		return;
	}

	m_menu.UpdateButtonState(BoxName, EButtonState::ButtonNormal);
}
