

#pragma once

class ACustomHUD;
class AHotbar;
#include "Inventory.generated.h"

/**
 * Responsible for drawing the HUD inventory (based on InventoryObject parameters)
 */
UCLASS()
class SPACEEXPLORER_API AInventory : public AActor
{
	GENERATED_UCLASS_BODY()

public:	
	void Init(ACustomHUD * pHUD, AHotbar * pHotbar, float fSlotSize = 64.f, float fInventoryBorder = 0.f);

	bool IsInvOpen();

	void DrawInventory();

	void UpdatePositions();

	void ToggleInventory(int32 nWidthCount, int32 nHeightCount);
	void OpenInventory(int32 nWidthCount, int32 nHeightCount);

	bool ItemDrag(bool bPickup);
	bool CheckMouseOver(const FName BoxName, bool bBegin);

private:
	/* flag whether inventory is open */
	UPROPERTY()
	bool m_bInvOpen;

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

	UPROPERTY()
	bool m_bCursorOverHitBox;

	// hitbox name cursor is hovering over - for inventory will be a number indicating the inventory index
	UPROPERTY()
	FName m_cursorOverHitBoxName;

private:
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
