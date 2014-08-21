

#pragma once

#include "GameFramework/HUD.h"
#include "CustomHUD.generated.h"

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
* Custom HUD class built by Claus
*/
UCLASS()
class SPACEEXPLORER_API ACustomHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	typedef void(ACustomHUD::*Func)();

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
	/* screen dimensions for HUD and menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
		FVector2D VScreenDimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
		float CurrentRatio;

	/* flag whether menu is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
		bool bMenuOpen;

	/* flag whether inventory is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
		bool bInventoryMode;

public:
	/* update the screen dimensions if changed and then update HUD and inventory*/
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
		void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	/* draw the HUD*/
	UFUNCTION(BlueprintCallable, Category = HUD)
		void DrawHUDComponents();

	/*
	// TODO: should offer an add button to blueprints, but must first figure out how to expose it to blueprints
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
		void AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size, Func& f);
	*/

	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
		void AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

	/* Draw HUD / Menu*/
	
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
		void OnReceiveDrawHUD(int32 SizeX, int32 SizeY);

	/* CustomHUD Events*/

	// BlueprintImplementableEvent allows these to be declared in the blueprint
	UFUNCTION(BlueprintImplementableEvent, Category = HUD_Menu)
		void MenuDrawCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
		void HUDDrawCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = HUD_Menu)
		void ReceiveHitBoxClickCompleted(const FName BoxName);

	UFUNCTION(BlueprintImplementableEvent, Category = HUD_Menu)
		void ReceiveHitBoxReleaseCompleted(const FName BoxName);

private:
	// list of all buttons in the main menu
	TArray<FButtonData> mainMenuButtons;

private:
	/* signal to draw the HUD or menu */
	virtual void ReceiveDrawHUD(int32 SizeX, int32 SizeY) override;
	/* signal for menu item interaction*/
	virtual void ReceiveHitBoxClick(const FName BoxName) override;
	virtual void ReceiveHitBoxRelease(const FName BoxName) override;
	
	/* menu */
	void DrawMenu(const TArray<FButtonData>& buttons);
	void DrawButton(const FButtonData& button);

	/* button actions*/
	void ReceiveHitBox(const FName BoxName, bool bClick);
	void Resume();
	void Quit();

	/* inventory */
	void SetInventoryPositions();
	void ItemDrag(bool bPickup);

	/* helpers*/
	float ScaleToScreensize();
	float ScaleToScreensize(float& OutSizeX, float& OutSizeY);

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;
};
