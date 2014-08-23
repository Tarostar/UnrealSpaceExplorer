

#include "SpaceExplorer.h"
#include "CustomHUD.h"
#include "Menu.h"

Menu::Menu()
{
	m_bMenuOpen = false;
	m_pHUD = NULL;
}

Menu::~Menu()
{
}

void Menu::Init(ACustomHUD * pHUD)
{
	m_pHUD = pHUD;
}

bool Menu::IsMenuOpen()
{
	return m_bMenuOpen;
}

/* Draw main menu */

void Menu::DrawMainMenu()
{
	int i;
	for (i = 0; i < m_mainMenuButtons.Num(); i++)
	{
		DrawButton(m_mainMenuButtons[i]);
	}
}

void Menu::DrawButton(const FButtonData& button)
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// Find middle of screen and subtract vertical offset
	// FVector2D menuStartLoc = VScreenDimensions / 2 - button.m_size.Y;

	FVector2D loc = button.m_location * m_pHUD->CurrentRatio;
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
	m_pHUD->DrawTextureSimple(texture, loc.X, loc.Y, button.m_scale);

	// draw text
	float width, height;
	m_pHUD->GetTextSize(button.m_text, width, height, button.m_font);

	// TODO: make colour configurable - either input to DrawButton or part of button struct
	FLinearColor textColor(0.391f, 0.735f, 0.213f, 1.0f);

	// draw text in middle of button, adjusting position by text size
	m_pHUD->DrawText(button.m_text, textColor, loc.X + button.m_size.X / 2 - width / 2, loc.Y + button.m_size.Y / 2 - height / 2, button.m_font);

	m_pHUD->AddHitBox(FVector2D(loc.X, loc.Y), button.m_size, button.m_hitboxName, false);

}

/* Generate main menu */

void Menu::AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size)
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
	funcs[0] = &Menu::Resume;
	funcs[1] = &Menu::Quit;

	// init main menu button number to text labels
	m_mainMenuButtons.Empty();
	m_mainMenuButtons.SetNum(texts.Num());

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
		m_mainMenuButtons[i] = button;
	}
}

// TODO: should offer an add button to blueprints, but must first figure out how to expose it to blueprints
/*
void Menu::AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size, Func& f)
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

m_mainMenuButtons.Add(button);
}
*/

/* Menu Events */

bool Menu::ReceiveHitBox(const FName boxName, bool bClick)
{
	// hitbox click or release

	// loop and see if any of the buttons fit the hitbox name, and execute its function
	int i;
	for (i = 0; i < m_mainMenuButtons.Num(); i++)
	{
		if (m_mainMenuButtons[i].m_hitboxName.Compare(boxName) == 0)
		{
			if (bClick)
			{
				// update to being pressed
				m_mainMenuButtons[i].m_buttonState = EButtonState::ButtonPressed;
				return true;
			}
			else if (m_mainMenuButtons[i].m_buttonState == EButtonState::ButtonPressed)
			{
				// button release: execute button action through its function pointer
				(this->* (m_mainMenuButtons[i].ButtonFunc))();
				return true;
			}
		}
	}

	return false;
}

void Menu::Resume()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// execute resume
	m_bMenuOpen = false;

	AGameMode* const GameMode = m_pHUD->GetWorld()->GetAuthGameMode();
	GameMode->ClearPause();
}

void Menu::Quit()
{
	if (m_pHUD == NULL)
	{
		return;
	}

	// execute quit
	m_pHUD->PlayerOwner->ConsoleCommand("Exit");
}

void Menu::UpdateButtonState(const FName boxName, EButtonState::Type buttonState)
{
	// loop and see if any of the buttons fit the hitbox name, and set its button state
	int i;
	for (i = 0; i < m_mainMenuButtons.Num(); i++)
	{
		if (m_mainMenuButtons[i].m_hitboxName.Compare(boxName) == 0)
		{
			m_mainMenuButtons[i].m_buttonState = buttonState;
		}
	}
}
