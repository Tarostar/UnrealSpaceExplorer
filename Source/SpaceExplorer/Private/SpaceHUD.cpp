

#include "SpaceExplorer.h"
#include "SpaceHUD.h"
#include "SpaceExplorerPawn.h"

#define BUTTONTYPE_UNDEFINED 		0
#define BUTTONTYPE_MAIN_RESTART 	1
#define BUTTONTYPE_MAIN_EXIT 		2

#define BUTTONTYPE_CONFIRM_YES 	1
#define BUTTONTYPE_CONFIRM_NO 	2

#define CANVAS_WHITE if(Canvas) Canvas->SetDrawColor(FColor_White);

//Cursor Draw Offset
//		use this to position texture over the point of your cursor, 
//			if the point is not at exactly 0,0
#define CURSOR_DRAW_OFFSET 3

//
//Static Consts
//

const FString ASpaceHUD::S_Title_Main = FString("Space Explorer");
const FString ASpaceHUD::S_Title_Confirm = FString("Exit Game?");

const FString ASpaceHUD::S_Button_Restart = FString("Restart");
const FString ASpaceHUD::S_Button_Exit = FString("Exit");

// Colors 
const FColor ASpaceHUD::FColorBlack = FColor(0, 0, 0, 255);
const FColor ASpaceHUD::FColorRed = FColor(255, 0, 0, 255);
const FColor ASpaceHUD::FColorYellow = FColor(255, 255, 0, 255);
const FColor ASpaceHUD::FColorBlue = FColor(0, 0, 255, 255);
const FColor ASpaceHUD::FColor_White = FColor(255, 255, 255, 255);
// Backgrounds 
const FLinearColor ASpaceHUD::LC_Black = FLinearColor(0, 0, 0, 1);
const FLinearColor ASpaceHUD::LC_Green = FLinearColor(0, 1, 0, 1);
const FLinearColor ASpaceHUD::LC_Red = FLinearColor(1, 0, 0, 1);
const FLinearColor ASpaceHUD::LC_Yellow = FLinearColor(1, 1, 0, 1);

ASpaceHUD::ASpaceHUD(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// Draw HUD?
	bDrawHUD = true;
	bDrawMainMenu = false;

	//States
	ConfirmDialogOpen = false;
	InMainMenu = true;

	//Scale
	GlobalHUDMult = 1;
	DefaultFontScale = 0.7;   //scaling down a size 36 font

	//	 I recommend creating fonts at a high resolution / size like 36
	//			then you can scale down the font as needed to any size of your choice

	// this avoids needing to make multiple fonts for different sizes, but have a high
	// resolution when you use larger font sizes
}

//Core 

void ASpaceHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//AddActiveHudButton("TEST", "BLAH", 100, 100, 150, 60);
	
	// how to get a ref to your custom PC
	//AYourPlayerController* YourChar = Cast<AYourPlayerController>(PlayerOwner);

	// how to get the character
	// ASpaceExplorerPawn* YourChar = Cast<ASpaceExplorerPawn>(GetOwningPawn());

}

//===============
// Draw Dialogs
//===============
void ASpaceHUD::DrawHUD_DrawDialogs()
{
	if (bDrawMainMenu)
		DrawMainMenu();

	if (ConfirmDialogOpen) 
		DrawConfirm("");
}

//Menus
void ASpaceHUD::DrawMainMenu()
{
	//Background
	DrawMaterialSimple(
		MaterialBackground,
		10, 10,
		256,
		512,
		1.3
		);

	//Menu Title

	//Draw buttons
	DrawMainMenuButtons();
}
void ASpaceHUD::DrawConfirm(const FString& Title)
{
	//Blue rect with alpha 50%
	DrawCustomRect(Canvas->SizeX / 2 - 100, Canvas->SizeY / 2 - 50, 200, 100, FLinearColor(0, 0, 1, 0.2333));

	//Confirm Title
	/*DrawCustomText(
		HUDFont, Title, xStart + 30, yStart + 20,
		LC_Black, DefaultFontScale,
		true, LC_Red
		);*/

	//Draw buttons
	DrawConfirmButtons();
}

//Buttons
void ASpaceHUD::DrawMainMenuButtons()
{
	// Start Point
	float xStart = 100;
	float yStart = 110;

	// Background
	VDrawTile(ButtonBackground, xStart, yStart, 150, 80, FColor(255, 255, 255, 120)); //alpha 120/255

	// Text
	DrawCustomText(
		HUDFont, "Restart", xStart + 30, yStart + 20,
		LC_Black, DefaultFontScale,
		true, LC_Red
		);

	// Struct
	// Add Button If Necessary
	// could be cleared and need refreshing if using a different menu
	// clear buttons with ButtonsMain.Empty()
	if (ButtonsMain.Num() < 1)
	{
		FCustomButtonStruct newButton = FCustomButtonStruct();
		newButton.type = BUTTONTYPE_MAIN_RESTART;
		newButton.toolTip = "Restart the Game!";
		newButton.x = xStart;
		newButton.width = xStart + 150;
		newButton.y = yStart;
		newButton.height = yStart + 80;

		//Add to correct array
		ButtonsMain.Add(newButton);
	}


	xStart = 100;
	yStart = 410;

	VDrawTile(ButtonBackground, xStart, yStart, 150, 80, FColor(255, 255, 255, 120)); //alpha 120/255

	//Text
	DrawCustomText(
		HUDFont, "Exit", xStart + 55, yStart + 20,
		LC_Black, DefaultFontScale,
		true, LC_Red
		);

	if (ButtonsMain.Num() < 2)
	{
		FCustomButtonStruct newButton = FCustomButtonStruct();
		newButton.type = BUTTONTYPE_MAIN_EXIT;
		newButton.toolTip = "Exit the Game!";
		newButton.x = xStart;
		newButton.width = xStart + 150;
		newButton.y = yStart;
		newButton.height = yStart + 80;

		//Add to correct array
		ButtonsMain.Add(newButton);
	}
}
void ASpaceHUD::DrawConfirmButtons()
{
	float xStart = Canvas->SizeX / 2 - 100;
	float yStart = Canvas->SizeY / 2 - 40;

	//Highlighted?
	if (ActiveButton_Type == BUTTONTYPE_CONFIRM_YES) ColorPtr = &LC_Green;
	else ColorPtr = &LC_Yellow;

	//Text
	DrawCustomText(
		HUDFont, "Yes", xStart + 30, yStart + 20,
		*ColorPtr, DefaultFontScale,
		true
		);

	if (ButtonsConfirm.Num() < 1)
	{
		FCustomButtonStruct newButton = FCustomButtonStruct();
		newButton.type = BUTTONTYPE_CONFIRM_YES;
		newButton.toolTip = "";
		newButton.x = xStart;
		newButton.width = 75;
		newButton.y = yStart + 20;
		newButton.height = 60;

		// could use GetTextSize to streamline this

		//Add to correct array
		ButtonsConfirm.Add(newButton);
	}

	xStart = Canvas->SizeX / 2 + 20;
	yStart = Canvas->SizeY / 2 - 40;

	//Highlighted?
	if (ActiveButton_Type == BUTTONTYPE_CONFIRM_NO) ColorPtr = &LC_Black;
	else ColorPtr = &LC_Yellow;

	//Text
	DrawCustomText(
		HUDFont, "No", xStart + 30, yStart + 20,
		*ColorPtr, DefaultFontScale,
		true
		);

	if (ButtonsConfirm.Num() < 2)
	{
		FCustomButtonStruct newButton = FCustomButtonStruct();
		newButton.type = BUTTONTYPE_CONFIRM_NO;
		newButton.toolTip = "";
		newButton.x = xStart;
		newButton.width = 75;
		newButton.y = yStart + 20;
		newButton.height = 60;

		// could use GetTextSize to streamline this

		//Add to correct array
		ButtonsConfirm.Add(newButton);
	}
}

//===============
// Cursor In Buttons
//===============
int32 ASpaceHUD::CheckCursorInButton(const TArray<FCustomButtonStruct>& ButtonArray)
{
	for (int32 b = 0; b < ButtonArray.Num(); b++)
	{
		CurCheckButton = &ButtonArray[b];

		//check cursor in bounds
		if (CurCheckButton->x <= MouseLocation.X && MouseLocation.X <= CurCheckButton->x + CurCheckButton->width &&
			CurCheckButton->y <= MouseLocation.Y && MouseLocation.Y <= CurCheckButton->x + CurCheckButton->height)
		{

			//Active Button Type
			ActiveButton_Type = CurCheckButton->type;

			//Tool Tip
			ActiveButton_Tip = CurCheckButton->toolTip;

			//Change Cursor
			CursorHoveringInButton = true;

			//Mouse Clicked?
			if (PlayerOwner->WasInputKeyJustPressed(EKeys::LeftMouseButton))
			{
				return ActiveButton_Type;
				//~~
				//no need to check rest of buttons
			}
		}
	}

	//No Click Occurred This Tick
	return -1;
}

//Check Confirm
void ASpaceHUD::CheckCursorInButtonsConfirm()
{
	//Check Confirm Buttons
	ClickedButtonType = CheckCursorInButton(ButtonsConfirm); //fills global ActiveButton_Type

	if (ClickedButtonType == BUTTONTYPE_CONFIRM_YES)
	{
		PlayerOwner->ConsoleCommand("Exit");
		return;
	}
	if (ClickedButtonType == BUTTONTYPE_CONFIRM_NO)
	{
		ConfirmDialogOpen = false;
		ButtonsConfirm.Empty(); //Buttons not needed anymore
		return;
	}
}

//Check Buttons
void ASpaceHUD::CheckCursorInButtonsMain()
{
	//Check Confirm Buttons
	ClickedButtonType = CheckCursorInButton(ButtonsMain);

	if (ClickedButtonType == BUTTONTYPE_MAIN_RESTART)
	{
		PlayerOwner->ConsoleCommand("RestartLevel");
		return;
	}
	if (ClickedButtonType == BUTTONTYPE_MAIN_EXIT)
	{
		ConfirmDialogOpen = true;
		return;
	}
}
void ASpaceHUD::DrawHUD_CheckCursorInButtons()
{
	if (ConfirmDialogOpen)
	{
		CheckCursorInButtonsConfirm();

		//Take Focus Away From All Other buttons
		return;
		//~
	}

	//Main
	CheckCursorInButtonsMain();
}

void ASpaceHUD::DrawToolTip()
{
	// if mouse is too far to right, draw from left instead
	float xStart = MouseLocation.X;
	float yStart = MouseLocation.Y + 5;

	// out vars
	float RV_xLength;
	float RV_yLength;

	// text size
	GetTextSize(
		ActiveButton_Tip,
		RV_xLength,
		RV_yLength,
		TooltipFont,
		DefaultFontScale * 2
		);

	// decide draw to Left or to the Right 

	// draw to the Left
	if (xStart + RV_xLength >= Canvas->SizeX - 40)
	{
		xStart -= 150 + 140 + 64 + RV_xLength;

		// if text is too long, bring it closer to the cursor
		if (xStart < 33) 
			xStart = 33;
	}

	// background
	/*DrawCustomRect(
		xStart, yStart,
		RV_xLength + 70,
		80,
		FLinearColor(0, 0, 1, 0.5) //alpha 0.7
		);*/

	// tool tip
	DrawText(
		ActiveButton_Tip,
		LC_Green,
		xStart, yStart + 20,
		TooltipFont,
		DefaultFontScale * 2,
		false		// scale position of message with HUD scale
		);
}
void ASpaceHUD::DrawHUD_DrawCursor()
{
	// tool tip
	if (ActiveButton_Tip != "") 
		DrawToolTip();

	// cursor hovering in a button?
	if (CursorHoveringInButton)
	{
		// pointer tex found?
		if (!CursorHoveringButton) 
			return;

		DrawFullSizeTile(CursorHoveringButton, MouseLocation.X - CURSOR_DRAW_OFFSET, MouseLocation.Y - CURSOR_DRAW_OFFSET, FColor_White);
	}

	else
	{
		// check if cursor texture found
		if (!CursorMain) 
			return;

		DrawFullSizeTile(CursorMain, MouseLocation.X - CURSOR_DRAW_OFFSET, MouseLocation.Y - CURSOR_DRAW_OFFSET, FColor_White);
	}
}

void ASpaceHUD::PlayerInputChecks()
{
	// check out this tutorial of mine for a list of all EKeys::
	// http://forums.epicgames.com/threads/972861-Tutorials-C-for-UE4-Code-Samples-gt-gt-New-Video-Freeze-Render-When-Tabbed-Out?p=31660286&viewfull=1#post31660286

	if (PlayerOwner->WasInputKeyJustPressed(EKeys::Escape) || PlayerOwner->WasInputKeyJustPressed(EKeys::M))
	{
		bDrawMainMenu = !bDrawMainMenu;
		SetCursorMoveOnly(false);
		return;
	}
	if (PlayerOwner->WasInputKeyJustPressed(EKeys::F))
	{
		SetCursorMoveOnly(!PlayerOwner->IsLookInputIgnored());
		return;
	}
	if (PlayerOwner->WasInputKeyJustPressed(EKeys::H))
	{
		bDrawHUD = !bDrawHUD;
		return;
	}

	//Confirm
	if (ConfirmDialogOpen)
	{
		if (PlayerOwner->WasInputKeyJustPressed(EKeys::Y))
		{
			PlayerOwner->ConsoleCommand("Exit");
			//could replace with function based on confirm context

			return;
		}
		if (PlayerOwner->WasInputKeyJustPressed(EKeys::N))
		{
			ConfirmDialogOpen = false;
			ButtonsConfirm.Empty(); //Buttons not needed anymore
			//Cancel Confirm

			return;
		}
	}
}

void ASpaceHUD::DrawHUD_Reset()
{
	ActiveButton_Type = -1;
	ActiveButton_Tip = "";
	CursorHoveringInButton = false;
}

void ASpaceHUD::DrawHUD()
{
	//==============================
	//==============================
	//==============================
	// abort if no player owner
	if (!PlayerOwner)
		return;

	//Multiplayer Safety Check
	if (!PlayerOwner->PlayerInput) 
		return; //not valid for first seconds of a multiplayer client
	//~~
	//==============================
	//==============================
	//==============================

	//Player Input
	PlayerInputChecks();

	//Draw HUD?
	if (!bDrawHUD) 
		return;
	
	//Super
	Super::DrawHUD();

	//No Canvas?
	if (!Canvas) 
		return;
	//

	//Reset States
	DrawHUD_Reset();

	//================
	//Get New Mouse Position
	//================
	PlayerOwner->GetMousePosition(MouseLocation.X, MouseLocation.Y);

	//Cursor In Buttons
	DrawHUD_CheckCursorInButtons();

	//Draw Dialogs
	DrawHUD_DrawDialogs();

	DrawActiveHud();

	//### Do Last ###
	//Draw Cursor
	DrawHUD_DrawCursor();

	//Debugging Info
	//PlayerOwner->ClientMessage("HUD Loop Completed!");
}

void ASpaceHUD::DrawActiveHud()
{
	for (int32 b = 0; b < ButtonsHud.Num(); b++)
	{
		DrawCustomText(HUDFont, ButtonsHud[b].label, ButtonsHud[b].x + 30, ButtonsHud[b].y + 20, LC_Yellow, DefaultFontScale, true);
	}
}

void ASpaceHUD::AddActiveHudButton(const FString& label, const FString& tooltip, float x, float y, float width, float height)
{
	FCustomButtonStruct newButton = FCustomButtonStruct();
	// TODO: need to set type...
	newButton.type = BUTTONTYPE_UNDEFINED;
	newButton.label = label;
	newButton.toolTip = tooltip;
	newButton.x = x;
	newButton.y = y;
	newButton.width = width;
	newButton.height = height;

	//Add to correct array
	ButtonsHud.Add(newButton);

	
}

