

#pragma once

class AActionBar;
class AInventory;
class AInventoryObject;
class ASpaceExplorerPawn;

#include "DragObject.h"
#include "Menu.h"
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
	/** PUBLIC FUNCTIONS */

	// Screen dimensions for HUD and menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	FVector2D VScreenDimensions;

	/* Flag whether menu is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
	bool m_bMenuOpen;

	/* Inventory class responsible for drawing inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	TArray<AInventory*> m_inventories;

	/* Responsible for drawing action bar */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	AActionBar * m_actionBar;

	/** PUBLIC VARIABLES */

	/* Update the screen dimensions if changed and then update HUD and inventory */
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	/* Get multiplier to scale HUD with */
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	float GetCurrentRatio();

	/* Toggle inventory on/off */
	UFUNCTION(BlueprintCallable, Category = HUD_Inventory)
	void ToggleInventory(int32 nID);
	void ToggleAllInventory();

	/* Draw the HUD and any displayed inventories and action bars */
	UFUNCTION(BlueprintCallable, Category = HUD)
	void DrawHUDComponents();

	/* Draw menu or HUD components and kick the blueprint implementable function MenuDrawCompleted or HUDDrawCompleted as appropriate */
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void OnReceiveDrawHUD(int32 SizeX, int32 SizeY);

	/* Calls menu class to auto generate the default main menu */
	UFUNCTION(BlueprintCallable, Category = HUD_Menu)
	void AutoGenerateMainMenu(FVector2D location, UTexture * textureNormal, UTexture * textureHover, UTexture * texturePressed, UFont* font, FVector2D size);

	/* Blueprint implementable function to draw HUD */
	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void DoDrawHUD();

	/* Blueprint implementable function to do any extra handling once menu draw is completed */
	UFUNCTION(BlueprintImplementableEvent, Category = HUD_Menu)
	void MenuDrawCompleted();

	/* Blueprint implementable function to do any extra handling once HUD, action bars and inventories have been drawn */
	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void HUDDrawCompleted();
	
	/* Drag and Drop */
	AInventoryObject* GetSourceInventoryObjectFromID(int32 nID);
	const class DragObject* GetDraggedItem();
	bool IsDragging();
	FString GetDraggedItemLabel();
	EActionType::Type GetDraggedItemType();
	int32 GetDraggedItemSlotIndex();
	int32 GetDraggedItemInventoryID();

	/* Execute action from HUD */
	void InvokeAction(class DragObject& item);

	/* If in HUD inventory hitbox invoke action and return true - otherwise return false */
	bool Interact();

	/* Pass right mouse button released or pressed (invoked from player pawn) */
	bool LMBRelease();
	bool LMBPressed();

	/* Pass right click event to HUD so that for example things can be removed from action bar */
	void Delete();

private:
	/** PRIVATE FUNCTIONS */

	/* Signal to draw the HUD or menu */
	virtual void ReceiveDrawHUD(int32 SizeX, int32 SizeY) override;

	/* Draw any item under the cursor being dragged from inventory */
	void DrawDraggedItem();

	/* Draw the HUD bars */
	void DrawHUDBars();

	/* Signal for menu item interaction */
	virtual void ReceiveHitBoxClick(const FName BoxName) override;
	virtual void ReceiveHitBoxRelease(const FName BoxName) override;
	virtual void ReceiveHitBoxBeginCursorOver(const FName BoxName) override;
	virtual void ReceiveHitBoxEndCursorOver(const FName BoxName) override;

	/* Returns scale factor multiplier */
	float ScaleToScreensize();

	/* Returns scale factor multiplier, and also takes X and Y value and scales them (convenience) */
	float ScaleToScreensize(float& OutSizeX, float& OutSizeY);

	/* After all game elements are created */
	virtual void PostInitializeComponents() override;

	/* On game start */
	virtual void BeginPlay() override;

	/* Get player reference */
	ASpaceExplorerPawn* GetSpaceExplorerPawn();

	/* Checks action bar and inventories if mouse hovering over a hitbox */
	bool IsHoveringOverHitbox();

	/** PRIVATE VARIABLES */

	/* Menu class responsible for drawing menu */
	Menu m_menu;
	
	/* Ratio used to scale everything on the HUD for screen size */
	float m_fCurrentRatio;

	/* Item being dragged */
	class DragObject m_draggedItem;

	/* Note: always use GetSpaceExplorerPawn()
	 * Holds the pawn for this HUD to avoid making repeated calls to retrieve it */
	ASpaceExplorerPawn* m_pSpaceExplorerPawn;
};
