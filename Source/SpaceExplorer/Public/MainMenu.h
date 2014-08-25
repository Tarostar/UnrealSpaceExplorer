

#pragma once

#include "Slate.h"

/**
 * 
 */
class SPACEEXPLORER_API SMainMenu : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SMainMenu) : _MainHUD()
	{
	}

	SLATE_ARGUMENT(TWeakObjectPtr<class AMainHUD>, MainHUD);

	SLATE_END_ARGS()

public:
	/**
	* Constructs and lays out the Main Menu UI Widget.
	*
	* \args Arguments structure that contains widget-specific setup information.
	**/
	void Construct(const FArguments& args);

	/**
	* Stores a weak reference to the HUD controlling this class.
	**/
	TWeakObjectPtr<class AMainHUD> MainHUD;

private:
	/**
	* Click handler for the Play Game! button - Calls MainHUD's PlayGameClicked() event.
	**/
	FReply PlayGameClicked();

	/**
	* Click handler for the Quit Game button - Calls MainHUD's QuitGameClicked() event.
	**/
	FReply QuitGameClicked();

	const struct FGlobalStyle* MenuStyle;
};
