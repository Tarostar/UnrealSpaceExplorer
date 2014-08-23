

#pragma once

class ACustomHUD;

namespace EButtonState
{

	enum Type
	{
		ButtonNormal,
		ButtonHover,
		ButtonPressed,
	};
}

namespace EButtonType
{
	enum Type
	{
		MainMenu,
	};
}

/**
 * Responsible for drawing the HUD menu
 */
class SPACEEXPLORER_API Menu
{
public:
	Menu();
	~Menu();

private:
	typedef void(Menu::*Func)();

	struct FButtonData
	{
		// displayed button text (would have preferred FText, but GetTextSize, etc. all expect FString)
		FString m_text;

		// hitbox name
		FName m_hitboxName;

		// button texture
		UTexture * m_textureNormal;
		UTexture * m_textureHover;
		UTexture * m_texturePressed;

		// button position
		FVector2D m_location;
		FVector2D m_size;

		// scale texture (default is 1)
		float m_scale;

		// used for textures and handling click/release
		EButtonState::Type m_buttonState;

		// button font
		UFont* m_font;

		// type - mostly for selecting texture
		EButtonType::Type m_type;

		Func ButtonFunc;

		// future?
		// FText toolTip;
		// float scale

		FButtonData()
		{
			m_text = TEXT("");
			m_hitboxName = "";
			m_location.X = 0;
			m_location.Y = 0;
			m_size.X = 256.0f;
			m_size.Y = 128.0f;
			m_scale = 1;
			m_buttonState = EButtonState::ButtonNormal;
			m_type = EButtonType::MainMenu;

			m_textureNormal = NULL;
			m_textureHover = NULL;
			m_texturePressed = NULL;

			//ButtonFunc = NULL;
		}
	};

public:
	void Init(ACustomHUD * pHUD);

	/* Creates the default main menu */
	void AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

	/* HUD has received either ReceiveHitBoxClick (bClick=true) or ReceiveHitBoxRelease (bClick=false) */
	bool ReceiveHitBox(const FName boxName, bool bClick);

	/* HUD has received either ReceiveHitBoxBeginCursorOver (ButtonHover) or ReceiveHitBoxEndCursorOver (ButtonNormal) */
	void UpdateButtonState(const FName boxName, EButtonState::Type buttonState);

	/* Draw the main menu */
	void DrawMainMenu();

	bool IsMenuOpen();

	/* Add custom buttons - should be accesible through the HUD from blueprints */
	// void AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size, Func& f);

private:
	TArray<FButtonData> m_mainMenuButtons;

	/* flag whether menu is open */
	bool m_bMenuOpen;

	ACustomHUD * m_pHUD;

private:
	void DrawButton(const FButtonData& button);

	/* Button actions*/
	void Resume();
	void Quit();
};
