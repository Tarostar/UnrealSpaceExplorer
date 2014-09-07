

#pragma once

class ACustomHUD;
class AHotbar;
class AInventoryObject;
#include "Inventory.generated.h"

/**
 * Responsible for drawing the HUD inventory (based on InventoryObject parameters)
 */
UCLASS()
class SPACEEXPLORER_API AInventory : public AActor
{
	GENERATED_UCLASS_BODY()

public:	
	// texture background
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_texture;

	// texture background when selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_textureSelected;

	/* flag whether inventory is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	bool m_bInvOpen;

	void Init(ACustomHUD * pHUD, AHotbar * pHotbar, float fSlotSize = 64.f, float fInventoryBorder = 0.f);

	bool IsInvOpen();

	void DrawInventory();

	void UpdatePositions();

	void ToggleInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory);
	void CloseInventory();
	void OpenInventory(AInventoryObject* pInventory, bool bInGroup);

	bool ItemDrag(bool bPickup);
	bool CheckMouseOver(const FName BoxName, bool bBegin);

private:
	UPROPERTY()
	ACustomHUD * m_pHUD;

	// pointer to hotbar for relative positioning when hotbar is visible
	UPROPERTY()
	AHotbar * m_pHotbar;

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

	// index of item being dragged (we don't remove from inventory, but stop drawing it until reset, moved or dropped into world)
	UPROPERTY()
	int32 m_nDraggingItemIndex;

	// true if several inventories drawn after each other
	bool m_bInGroup;

	UPROPERTY()
	AInventoryObject* m_pInventory;

	// optional pointer to previously drawn inventory for calculating position
	UPROPERTY()
	AInventory* m_pPreviousInventory;

private:
	void SetStartPosition();
	void SetHitBoxPositionArray();
	void DrawDraggedItem();
};
