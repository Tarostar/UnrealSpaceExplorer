

#include "SpaceExplorer.h"
#include "CustomHUD.h"

const float DEFAULT_SCREEN_WIDTH = 2560.0f;
const float DEFAULT_SCREEN_HEIGHT = 2560.0f;

ACustomHUD::ACustomHUD(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

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
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, TEXT("UpdateScreenDimensions"));

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

	SetInventoryPositions();
}

void ACustomHUD::DrawMenu(const TArray<FButtonData>& buttons)
{
	// Find middle of screen and subtract vertical offset
	FVector2D menuStartLoc = VScreenDimensions / 2 - FVector2D(0.0f, 64.0f);
	
	int i;
	for (i = 0; i < buttons.Num(); i++)
	{
		DrawButton(buttons[i]);
	}	
}

void ACustomHUD::DrawButton(const FButtonData& button)
{
	//TODO: offset? - i.e. subtract half of button size, e.g. 128x64

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
	DrawTextureSimple(texture, button.m_location.X, button.m_location.Y, button.m_scale);

	// draw text
	float width, height;
	GetTextSize(button.m_text, width, height, button.m_font);

	// divide by two and subtract offset
	width /= 2;
	height /= 2;

	// TODO: make colour configurable - either input to DrawButton or part of button struct
	FLinearColor textColor(0.391f, 0.735f, 0.213f, 1.0f);

	DrawText(button.m_text, textColor, width, height, button.m_font);

	// hitbox
	FVector2D pos(button.m_location.X, button.m_location.Y);
	// TODO: set hitbox size to texture size or at least allow it to be set
	FVector2D size(256.0f, 128.0f);

	AddHitBox(pos, size, button.m_hitboxName, false);

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
	float sizeX, sizeY, ratio;
	ScaleToScreensize(sizeX, sizeY, ratio);

	// TODO...
}

void ACustomHUD::ScaleToScreensize(float& ratio)
{
	// set ratio from width
	ratio = VScreenDimensions.X / DEFAULT_SCREEN_WIDTH;

	// if height is a smaller ratio use that instead
	if (ratio > VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT)
		ratio = VScreenDimensions.Y / DEFAULT_SCREEN_HEIGHT;
}

void ACustomHUD::ScaleToScreensize(float& sizeX, float& sizeY, float& ratio)
{
	ScaleToScreensize(ratio);
		
	sizeX *= ratio;
	sizeY *= ratio;
}

/* Menu */

void ACustomHUD::AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font)
{
	FButtonData button;

	button.m_text = text;
	button.m_hitboxName = hitboxName;
	button.m_location = location;
	button.m_scale = 1;
	button.m_buttonState = EButtonState::ButtonNormal;
	button.m_type = EButtonType::MainMenu;

	button.m_textureNormal = textureNormal;
	button.m_textureHover = textureHover;
	button.m_texturePressed = texturePressed;

	button.m_font = font;

	mainMenuButtons.Add(button);
}
