

#include "SpaceExplorer.h"
#include "MainMenu.h"
#include "MainHUD.h"
#include "GlobalMenuStyle.h"
#include "MenuStyles.h"

void SMainMenu::Construct(const FArguments& args)
{
	MainHUD = args._MainHUD;

	MenuStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");

	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			[
				SNew(STextBlock)
				.TextStyle(&MenuStyle->MenuTitleStyle)
				.Text(FText::FromString("Main Menu"))
			]
			+ SOverlay::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.ButtonStyle(&MenuStyle->MenuButtonStyle)
						.TextStyle(&MenuStyle->MenuButtonTextStyle)
						.Text(FText::FromString("Play Game!"))
						.OnClicked(this, &SMainMenu::PlayGameClicked)
					]
					+ SVerticalBox::Slot()
					[
						SNew(SButton)
						.ButtonStyle(&MenuStyle->MenuButtonStyle)
						.TextStyle(&MenuStyle->MenuButtonTextStyle)
						.Text(FText::FromString("Quit Game"))
						.OnClicked(this, &SMainMenu::QuitGameClicked)
					]
			]
		];
}

FReply SMainMenu::PlayGameClicked()
{
	MainHUD->PlayGameClicked();
	return FReply::Handled();
}

FReply SMainMenu::QuitGameClicked()
{
	MainHUD->QuitGameClicked();
	return FReply::Handled();
}
