

#pragma once

#include "GameFramework/HUD.h"
#include "SpaceHUD.generated.h"


USTRUCT()
struct FCustomButtonStruct
{
	GENERATED_USTRUCT_BODY()

	// text
	FString		text;

	// background
	bool		bVisible;

	// button
	int32 		type;
	FString		label;
	FString		toolTip;
	float 		x;
	float 		y;
	float 		width;
	float 		height;

	//default properties

	FCustomButtonStruct()
	{
		type = -1;
		text = "";
		toolTip = "";
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}
};

/**
 * This HUD class is built from the example HUD created by RAMA https://wiki.unrealengine.com/User:Rama - Thank you for sharing!
 */
UCLASS()
class SPACEEXPLORER_API ASpaceHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	// Font 
	//		I recommend creating the font at a high resolution / size like 36
	//			then you can scale down the font as needed to any size of your choice

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomHUD)
	UFont* HUDFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomHUD)
	UFont* TooltipFont;

	/** Font Scaling Used By Your HUD Code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomHUD)
	float DefaultFontScale;

	/** HUD Scaling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CustomHUD)
	float GlobalHUDMult;

	// T2D 
	/** Cursor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = T2D)
	UTexture2D* CursorMain;

	/** Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = T2D)
	UTexture2D* CursorHoveringButton;

	/** Button */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = T2D)
	UTexture2D* ButtonBackground;

	// Materials 
	/** Events */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	UMaterialInterface* MaterialBackground;
	
	// Options

	/* Draw Hud? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Options)
	bool bDrawHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Options)
	bool bDrawMainMenu;

	//Cursor
public:
	FVector2D MouseLocation;
	void DrawHUD_DrawCursor();

	//Buttons
public:
	TArray<FCustomButtonStruct> ButtonsMain;
	TArray<FCustomButtonStruct> ButtonsConfirm;

	TArray<FCustomButtonStruct> ButtonsHud;

	//Cursor In buttons
	void DrawHUD_CheckCursorInButtons();
	void CheckCursorInButtonsMain();
	void CheckCursorInButtonsConfirm();

	const FCustomButtonStruct* CurCheckButton;
	int32 CheckCursorInButton(const TArray<FCustomButtonStruct>& ButtonArray);
	int32 ClickedButtonType;
	//States
	bool ConfirmDialogOpen;
	bool InMainMenu;

	int32 		ActiveButton_Type;
	FString 	ActiveButton_Tip;
	bool CursorHoveringInButton;
	//Colors
public:
	const FLinearColor * ColorPtr;

	// Colors
	static const FColor		FColorBlack;
	static const FColor		FColorRed;
	static const FColor		FColorYellow;
	static const FColor		FColorBlue;
	static const FColor		FColor_White;

	static const FLinearColor LC_Black;
	static const FLinearColor LC_Green;
	static const FLinearColor LC_Red;
	static const FLinearColor LC_Yellow;
public:

	// Titles
	static const FString S_Title_Main;
	static const FString S_Title_Confirm;
	// Button Text
	static const FString S_Button_Restart;
	static const FString S_Button_Exit;

	// Utility 

	// Stop Camera From Moving With Mouse
	FORCEINLINE void SetCursorMoveOnly(bool CursorOnly)
	{
		if (!PlayerOwner) 
			return;
		
		PlayerOwner->SetIgnoreLookInput(CursorOnly);

	}

	// DrawLine
	FORCEINLINE void DrawCustomLine
	(
		const FVector2D& Start,
		const FVector2D& End,
		const FLinearColor& TheColor,
		const float& Thick
		)
	{
		if (!Canvas) return;
		//

		FCanvasLineItem NewLine(Start, End);
		NewLine.SetColor(TheColor);
		NewLine.LineThickness = Thick;
		Canvas->DrawItem(NewLine);
	}

	//~

	FORCEINLINE void DrawCustomRect(
		float X, float Y,
		float Width, float Height,
		const FLinearColor& Color
		)
	{
		if (!Canvas) return;
		//

		FCanvasTileItem RectItem(
			FVector2D(X, Y),
			FVector2D(Width, Height),
			Color
			);

		RectItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(RectItem);
	}

	//~

	//DrawText
	FORCEINLINE void DrawCustomText(
		UFont*	TheFont,
		const FString& TheStr,
		const float& X, const float& Y,
		const FLinearColor& TheColor,
		const float& TheScale,
		bool DrawOutline = false,
		const FLinearColor OutlineColor = FLinearColor(0, 0, 0, 1)
		) {
		if (!Canvas) return;
		//

		//Text and Font
		FCanvasTextItem NewText(
			FVector2D(X, Y),
			FText::FromString(TheStr),
			TheFont,
			TheColor
			);

		//Text Scale
		NewText.Scale.Set(TheScale, TheScale);

		//Outline gets its alpha from the main color
		NewText.bOutlined = true;
		NewText.OutlineColor = OutlineColor;
		NewText.OutlineColor.A = TheColor.A * 2;

		//Draw
		Canvas->DrawItem(NewText);
	}

	//~
	//Draw Full Size Tile
	FORCEINLINE void DrawFullSizeTile(UTexture2D* tex, float x, float y, const FColor& Color)
	{
		if (!Canvas) return;
		if (!tex) return;
		//~~

		Canvas->SetDrawColor(Color);

		//Draw
		Canvas->DrawTile(
			tex, x, y, //z pos
			tex->GetSurfaceWidth(), //screen width
			tex->GetSurfaceHeight(),  //screen height
			0, //texture start width
			0, //texture start height
			tex->GetSurfaceWidth(), //texture width from start
			tex->GetSurfaceHeight(), //texture height from start
			BLEND_Translucent
			);
	}

	//~

	FORCEINLINE void VDrawTile(UTexture2D* tex, float x, float y, float screenX, float screenY, const FColor& TheColor)
	{
		if (!Canvas) return;
		if (!tex) return;
		//~

		Canvas->SetDrawColor(TheColor);

		//Draw
		Canvas->DrawTile(
			tex, x, y, //z pos
			screenX, //screen width
			screenY,  //screen height
			0, //texture start width
			0, //texture start height
			tex->GetSurfaceWidth(), //texture width from start
			tex->GetSurfaceHeight(), //texture height from start
			BLEND_Translucent
			);
	}

	//~

	//Draw
public:
	void DrawHUD_DrawDialogs();

	//Menus
	void DrawMainMenu();
	void DrawConfirm(const FString& Title);

	//Buttons
	void DrawMainMenuButtons();
	void DrawConfirmButtons();
public:
	void DrawToolTip();

	// blueprint interface to active HUD
	//UFUNCTION(BlueprintImplementableEvent, Category = ActiveHUD)
	UFUNCTION(BlueprintCallable, Category = ActiveHUD)
	void AddActiveHudButton(const FString& label, const FString& tooltip, float x, float y, float width, float height);

	//Core
public:
	void PlayerInputChecks();
protected:
	//Draw HUD
	void DrawHUD_Reset();
	virtual void DrawHUD() override;
	void DrawActiveHud();

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;


};
