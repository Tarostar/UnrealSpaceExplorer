

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
	// screen dimensions for HUD and menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Screen)
	FVector2D VScreenDimensions;

	/* flag whether menu is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Menu)
	bool m_bMenuOpen;
	
	// TODO: this will eventually need to be an array (one for each object in the pawn)...

	// inventory class responsible for drawing inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	TArray<AInventory*> m_inventories;

	// responsible for drawing action bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	AActionBar * m_actionBar;

	/* update the screen dimensions if changed and then update HUD and inventory*/
	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	void UpdateScreenDimensions(int32 SizeX, int32 SizeY);

	UFUNCTION(BlueprintCallable, Category = HUD_Screen)
	float GetCurrentRatio();

	/* toggle inventory on/off */
	/*  top, bottom, right parameters are position of previous inventory, -1 if drawing only single inventory */
	UFUNCTION(BlueprintCallable, Category = HUD_Inventory)
	void ToggleInventory(int32 nID);
	void ToggleAllInventory();

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

	/* Drag and Drop */
	AInventoryObject* GetSourceInventoryObjectFromID(int32 nID);
	const class DragObject* GetDraggedItem();
	bool IsDragging();
	FString GetLabel();
	EActionType::Type GetType();
	int32 GetSlotIndex();
	int32 GetInventoryID();

	/* Execute action from HUD */
	void InvokeAction(class DragObject& item);

	/* if in HUD inventory hitbox invoke action and return true - otherwise return false */
	bool Interact();

	/* right mouse button released */
	bool LMBRelease();
	bool LMBPressed();

	/* pass right click event to HUD so that for example things can be removed from action bar */
	void Delete();

private:
	// menu class responsible for drawing menu
	Menu m_menu;
	
	// ratio used to scale everything on the HUD for screen size
	float m_fCurrentRatio;

	// item being dragged
	class DragObject m_draggedItem;

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

	void DrawDraggedItem();
	ASpaceExplorerPawn* GetSpaceExplorerPawn();

	bool IsHoveringOverHitbox();
};
