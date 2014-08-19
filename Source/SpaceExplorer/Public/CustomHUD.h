

#pragma once

#include "GameFramework/HUD.h"
#include "CustomHUD.generated.h"

enum EButtonState
{
	ButtonNormal,
	ButtonHover,
	ButtonPressed,
};

enum EButtonType
{
	MainMenu,
};

USTRUCT()
struct FButtonData
{
	GENERATED_USTRUCT_BODY()

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

	// mainly for displaying button texture appropriate to its state
	EButtonState m_buttonState;

	// button font
	UFont* m_font;

	// type - mostly for selecting texture
	EButtonType m_type;

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
	}
};

/**
* Custom HUD class built by Claus
*/
UCLASS()
class SPACEEXPLORER_API ACustomHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public: // TODO: review 
	// list of all buttons in the main menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
	TArray<FButtonData> mainMenuButtons;

private:
	/* signal to draw the HUD or menu */
	//virtual void ReceiveDrawHUD(int32 SizeX, int32 SizeY) override;
	
	/* menu */
	void DrawMenu(const TArray<FButtonData>& buttons);

	/* inventory */
	void SetInventoryPositions();

	/* helpers*/
	float ScaleToScreensize();
	float ScaleToScreensize(float& sizeX, float& sizeY);

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;


public:
	/* screen dimensions for HUD and menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	FVector2D VScreenDimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	float CurrentRatio;

	/* flag whether menu is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
	bool bMenuOpen2;

public:
	/* update the screen dimensions if changed and then update HUD and inventory*/
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	/* draw the HUD*/
	UFUNCTION(BlueprintCallable, Category = HUD)
	void DrawHUDComponents();

	/* add main menu button*/
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void AddMainButton(FVector2D location, const FString& text, const FName& hitboxName, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
		void AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

	/* draw button*/
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void DrawButton(const FButtonData& button);

	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void OnReceiveDrawHUD(int32 SizeX, int32 SizeY);
};
