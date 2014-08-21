

#include "SpaceExplorer.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

/* Init */

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	CurrentRatio = 1.0f;
	bMenuOpen = false;
	bInventoryMode = false;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
}

void ACustomHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// init ?
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
	
	if (bMenuOpen)
	{
		// menu open - draw main menu
		DrawMenu(mainMenuButtons);

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

void ACustomHUD::DrawMenu(const TArray<FButtonData>& buttons)
{
	int i;
	for (i = 0; i < buttons.Num(); i++)
	{
		DrawButton(buttons[i]);
	}	
}

void ACustomHUD::DrawButton(const FButtonData& button)
{
	// Find middle of screen and subtract vertical offset
	// FVector2D menuStartLoc = VScreenDimensions / 2 - button.m_size.Y;

	FVector2D loc = button.m_location * CurrentRatio;
	// TODO: use DrawTexture and also scale button size to ratio
	// -> need to figure this out: Texture-space width of the quad (in normalized UV distance).
	// FVector2D size = button.m_size * CurrentRatio;
	
	// set correct texture
	UTexture* texture;
	switch (button.m_buttonState)
	{
		case EButtonState::ButtonNormal: texture = button.m_textureNormal;
			break;
		case EButtonState::ButtonHover: texture = button.m_textureHover;
			break;
		case EButtonState::ButtonPressed: texture = button.m_texturePressed;
			break;
		default: // something went wrong, invalid button state
			// TODO: handle error
			return;
	}
	
	// draw button texture
	DrawTextureSimple(texture, loc.X, loc.Y, button.m_scale);

	// draw text
	float width, height;
	GetTextSize(button.m_text, width, height, button.m_font);

	// TODO: make colour configurable - either input to DrawButton or part of button struct
	FLinearColor textColor(0.391f, 0.735f, 0.213f, 1.0f);

	// draw text in middle of button, adjusting position by text size
	DrawText(button.m_text, textColor, loc.X + button.m_size.X / 2 - width / 2, loc.Y + button.m_size.Y / 2 - height / 2, button.m_font);

	AddHitBox(FVector2D(loc.X, loc.Y), button.m_size, button.m_hitboxName, false);

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

/* Build Menu */

void ACustomHUD::AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size)
{
	// auto generate main menu based on starting location, size, textures and font

	// TODO: this needs to be written in more elegant and robust code
	// this list determines names and number of buttons
	TArray<FString> texts;
	texts.SetNum(2);
	texts[0] = "Resume";
	texts[1] = "Quit";

	TArray<Func> funcs;
	funcs.SetNum(2);
	funcs[0] = &ACustomHUD::Resume;
	funcs[1] = &ACustomHUD::Quit;

	// init main menu button number to text labels
	mainMenuButtons.Empty();
	mainMenuButtons.SetNum(texts.Num());

	// default button setup
	FButtonData button;
	button.m_location = location;
	button.m_scale = 1;
	button.m_buttonState = EButtonState::ButtonNormal;
	button.m_type = EButtonType::MainMenu;
	button.m_size = size;
	button.m_textureNormal = textureNormal;
	button.m_textureHover = textureHover;
	button.m_texturePressed = texturePressed;
	button.m_font = font;

	int i;
	for (i = 0; i < texts.Num(); i++)
	{
		// override for each button

		// arrange vertically, with half the space of a button between each button
		button.m_location.Y += size.Y * 1.5f * i;
		button.m_text = texts[i];
		button.m_hitboxName = FName(*texts[i]);
		button.ButtonFunc = funcs[i];

		// add to button array
		mainMenuButtons[i] = button;
	}
}

// TODO: should offer an add button to blueprints, but must first figure out how to expose it to blueprints
/*
void ACustomHUD::AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size, Func& f)
{
	FButtonData button;

	button.m_text = text;
	button.m_hitboxName = hitboxName;
	button.m_location = location;
	button.m_scale = 1;
	button.m_buttonState = EButtonState::ButtonNormal;
	button.m_type = EButtonType::MainMenu;

	button.m_size = size;

	button.m_textureNormal = textureNormal;
	button.m_textureHover = textureHover;
	button.m_texturePressed = texturePressed;

	button.m_font = font;

	button.ButtonFunc = f;

	mainMenuButtons.Add(button);
}
*/

/* Menu Events */

void ACustomHUD::ReceiveHitBoxClick(const FName BoxName)
{
	ReceiveHitBox(BoxName, true);
}

void ACustomHUD::ReceiveHitBoxRelease(const FName BoxName)
{
	ReceiveHitBox(BoxName, false);	
}

void ACustomHUD::ReceiveHitBox(const FName BoxName, bool bClick)
{
	// hitbox click or release
	
	// loop and see if any of the buttons fit the hitbox name, and execute its function
	int i = 0;
	while (i < mainMenuButtons.Num())
	{
		if (mainMenuButtons[i].m_hitboxName.Compare(BoxName) == 0)
		{
			if (bClick)
			{

				// update to being pressed
				mainMenuButtons[i].m_buttonState = EButtonState::ButtonPressed;
				return;
			}
			else if (mainMenuButtons[i].m_buttonState == EButtonState::ButtonPressed)
			{
				// button release: execute button action through its function pointer
				(this->* (mainMenuButtons[i].ButtonFunc))();
				return;
			}
		}

		// next hitbox
		i++;
	}

	if (bInventoryMode)
	{
		// in item mode, check for item pickup
		ItemDrag(bClick);
	}
}

void ACustomHUD::Resume()
{
	// execute resume
	 bMenuOpen = false;

	AGameMode* const GameMode = GetWorld()->GetAuthGameMode();
	GameMode->ClearPause();
}

void ACustomHUD::Quit()
{
	// execute quit
	PlayerOwner->ConsoleCommand("Exit");
}
/*
bool ACustomHUD::SetPause(bool bPause, FCanUnpause CanUnpauseDelegate)
{
	bool bResult = false;
	if (GetNetMode() != NM_Client)
	{
		AGameMode* const GameMode = GetWorld()->GetAuthGameMode();
		if (bPause)
		{
			// Pause gamepad rumbling too if needed
			bResult = GameMode->SetPause(this, CanUnpauseDelegate);
		}
		else
		{
			GameMode->ClearPause();
		}
	}
	return bResult;
}
*/
