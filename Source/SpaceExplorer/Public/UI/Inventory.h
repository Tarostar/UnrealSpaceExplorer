

#pragma once

class AInventoryObject;
class AActionBar;

#include "CustomHUD.h"
#include "Inventory.generated.h"

/**
 * Responsible for drawing the HUD inventory (based on InventoryObject parameters)
 */
UCLASS()
class SPACEEXPLORER_API AInventory : public AActor
{
	GENERATED_UCLASS_BODY()

public:	
	/** PUBLIC FUNCTIONS */

	/* Reference to spawning HUD, reference to any action bar (optional - for draw positioning), size of slot, size of border */
	void Init(ACustomHUD * pHUD, AActionBar * pActionBar, float fSlotSize = 64.f, float fInventoryBorder = 0.f);

	/* Is the inventory open */
	bool IsInvOpen();

	/* Draws the inventory */
	void DrawInventory();

	/* Update position of inventory by calling SetStartPosition and SetHitBoxPositionArray*/
	void UpdatePositions();

	/* Methods for opening, closing and moving inventories */
	void ToggleInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory);
	void CloseInventory();
	void OpenInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory);
	void Move(bool bInGroup, AInventory* pPreviousInventory);
	
	/* Unique ID for this inventory */
	int32 GetID();

	/* Pick up an item to drag it, or drop a dragged item */
	bool ItemDrag(bool bPickup, class DragObject& item);

	/* Check if mouse over an inventory hitbox slot */
	bool CheckMouseOver(const FName BoxName, bool bBegin);

	/* Invoke action for inventory in the current slot being hovered over*/
	bool InvokeAction();

	/* Check if this inventory has a slot being hovered over */
	bool ActiveHitbox();

	/** PUBLIC VARIABLES */

	/* Texture background */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_texture;

	/* Texture background when selected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_textureSelected;

	/* Flag whether inventory is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	bool m_bInvOpen;

private:
	/** PRIVATE FUNCTIONS */

	void SetStartPosition();
	void SetHitBoxPositionArray();

	/** PRIVATE VARIABLES */

	UPROPERTY()
	ACustomHUD * m_pHUD;

	UPROPERTY()
	AActionBar * m_actionBar;

	UPROPERTY()
	FVector2D m_vInvStartpos;

	UPROPERTY()
	float m_fSlotSize;

	UPROPERTY()
	float m_fInventoryBorder;

	UPROPERTY()
	int32 m_nWidthCount;

	UPROPERTY()
	int32 m_nHeightCount;

	UPROPERTY()
	TArray<FVector2D> m_vaInvHitBoxPositions;

	// slot mouse cursor is hovering over
	UPROPERTY()
	int32 m_nHoverIndex;

	// true if several inventories drawn after each other
	UPROPERTY()
	bool m_bInGroup;

	UPROPERTY()
	AInventoryObject* m_pInventory;

	// optional pointer to previously drawn inventory for calculating position
	UPROPERTY()
	AInventory* m_pPreviousInventory;
};
