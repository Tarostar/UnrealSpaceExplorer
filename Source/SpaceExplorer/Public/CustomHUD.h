

#pragma once

#include "Hotbar.h"
#include "Menu.h"
#include "Inventory.h"
#include "InventoryObject.h"
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
	// screen dimensions for HUD and menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	FVector2D VScreenDimensions;

	/* flag whether menu is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
	bool m_bMenuOpen;
	
	// TODO: this will eventually need to be an array (one for each object in the pawn)...

	// inventory class responsible for drawing inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	AInventory * m_inventory;

	/* update the screen dimensions if changed and then update HUD and inventory*/
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	float GetCurrentRatio();

	/* toggle inventory on/off */
	/*  top, bottom, right parameters are position of previous inventory, -1 if drawing only single inventory */
	UFUNCTION(BlueprintCallable, Category = HUD_Inventory)
	void ToggleInventory(AInventoryObject* pInventory, bool bInGroup);

	/* draw the HUD*/
	UFUNCTION(BlueprintCallable, Category = HUD)
	void DrawHUDComponents();

	/* Draw HUD / Menu*/
	
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void OnReceiveDrawHUD(int32 SizeX, int32 SizeY);

	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

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

	// hotbar class responsible for drawing hotbar
	UPROPERTY()
	AHotbar * m_hotbar;

	// ratio used to scale everything on the HUD for screen size
	float m_fCurrentRatio;

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

	virtual void BeginPlay() override;
};
