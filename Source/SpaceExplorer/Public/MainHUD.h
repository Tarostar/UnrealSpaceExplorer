

#pragma once

#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

/**
 * 
 */
UCLASS()
class SPACEEXPLORER_API AMainHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Initializes the Slate UI and adds it as widget content to the game viewport.
	**/
	virtual void PostInitializeComponents() override;

	/**
	* Called by SMainMenu whenever the Play Game! button has been clicked.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Menus|Main Menu")
	void PlayGameClicked();

	/**
	* Called by SMainMenu whenever the Quit Game button has been clicked.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Menus|Main Menu")
	void QuitGameClicked();

private:
	/**
	* Reference to the Main Menu Slate UI.
	**/
	TSharedPtr<class SMainMenu> MainMenu;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Reference to an SCompoundWidget, TSharedPtr adds to the refcount of MyUIWidget
	/////MyUIWidget will not self-destruct as long as refcount > 0
	/////MyUIWidget refcount will be (refcout-1) if HUD is destroyed.
	/*TSharedPtr<class SMyUIWidget> MyUIWidget;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/////Called as soon as game starts, create SCompoundWidget and give Viewport access
	void BeginPlay();*/
};
