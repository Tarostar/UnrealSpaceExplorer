

#include "SpaceExplorer.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 1440.0f;

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	CurrentRatio = 1.0f;
	bMenuOpen2 = false;
	VScreenDimensions.Set(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
}

/* Init */
void ACustomHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// init ?
}



/* Draw */

/*void ACustomHUD::ReceiveDrawHUD(int32 SizeX, int32 SizeY)
{
	OnReceiveDrawHUD(int32 SizeX, int32 SizeY);
}*/

void ACustomHUD::OnReceiveDrawHUD(int32 SizeX, int32 SizeY)
{
	if (!FMath::IsNearlyEqual(VScreenDimensions.X, SizeX, 0.000001f) || !FMath::IsNearlyEqual(VScreenDimensions.Y, SizeY, 0.000001f))
	{
		UpdateScreenDimensions(SizeX, SizeY);
	}
	
	if (bMenuOpen2)
	{

		// menu open - draw main menu
		DrawMenu(mainMenuButtons);
		return;
	}

	// draw HUD and inventory
	DrawHUDComponents();	
}

void ACustomHUD::UpdateScreenDimensions(int32 SizeX, int32 SizeY)
{
	VScreenDimensions.Set(SizeX, SizeY);

	CurrentRatio = ScaleToScreensize();

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
		case ButtonNormal: texture = button.m_textureNormal;
			break;
		case ButtonHover: texture = button.m_textureHover;
			break;
		case ButtonPressed: texture = button.m_texturePressed;
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

/* Helpers*/

void ACustomHUD::SetInventoryPositions()
{
	// TODO...
}

float ACustomHUD::ScaleToScreensize()
{
	// set ratio from width
	float ratio = VScreenDimensions.X / DEFAULT_SCREEN_WIDTH;

	// if height is a smaller ratio use that instead
	if (ratio > VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT)
		ratio = VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT;

	return ratio;
}

float ACustomHUD::ScaleToScreensize(float& sizeX, float& sizeY)
{
	float ratio = ScaleToScreensize();

	// scale X and Y to ratio
	sizeX *= ratio;

	return ratio;
	sizeY *= ratio;
}

/* Menu */

void ACustomHUD::AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size)
{
	// auto generate main menu based on starting location, size, textures and font

	// this list determines names and number of buttons
	TArray<FString> texts;
	texts.SetNum(2);
	texts[0] = "Resume";
	texts[1] = "Quit";

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

		// add to button array
		mainMenuButtons[i] = button;
	}
}

void ACustomHUD::AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size)
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

	mainMenuButtons.Add(button);
}
