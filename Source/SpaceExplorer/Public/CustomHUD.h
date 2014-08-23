

#pragma once

#include "Hotbar.h"
#include "Menu.h"
#include "Inventory.h"
#include "GameFramework/HUD.h"
#include "CustomHUD.generated.h"

/**
* Custom HUD class built by Claus
*/
UCLASS()
class SPACEEXPLORER_API ACustomHUD : public AHUD
{
	GENERATED_UCLASS_BODY()
	
public:
	/* screen dimensions for HUD and menu */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	FVector2D VScreenDimensions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	float CurrentRatio;

	/* cursor over hitbox */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	bool bCursorOverHitBox;

	/* hitbox name cursor is hovering over - for inventory will be a number indicating the inventory index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	FName CursorOverHitBoxName;

public:
	/* update the screen dimensions if changed and then update HUD and inventory*/
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	/* draw the HUD*/
	UFUNCTION(BlueprintCallable, Category = HUD)
	void DrawHUDComponents();

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
	// menu class responsible for drawing menu
	Menu m_menu;

	// inventory class responsible for drawing inventory
	Inventory m_inventory;

	// hotbar class responsible for drawing hotbar
	Hotbar m_hotbar;

private:
	/* signal to draw the HUD or menu */
	virtual void ReceiveDrawHUD(int32 SizeX, int32 SizeY) override;
	/* signal for menu item interaction*/
	virtual void ReceiveHitBoxClick(const FName BoxName) override;
	virtual void ReceiveHitBoxRelease(const FName BoxName) override;
	virtual void ReceiveHitBoxBeginCursorOver(const FName BoxName) override;
	virtual void ReceiveHitBoxEndCursorOver(const FName BoxName) override;
	
	/* helpers*/
	float ScaleToScreensize();
	float ScaleToScreensize(float& OutSizeX, float& OutSizeY);

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;
};
