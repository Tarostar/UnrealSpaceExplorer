

#pragma once

class ACustomHUD;
class Hotbar;

/**
 * Responsible for drawing the HUD inventory (based on InventoryObject parameters)
 */
class SPACEEXPLORER_API Inventory
{
public:
	Inventory();
	~Inventory();
	
	void Init(ACustomHUD * pHUD, Hotbar * pHotbar, float fSlotSize = 64.f, float fInventoryBorder = 0.f);

	bool IsInvOpen();

	void DrawInventory();

	void UpdatePositions();

	void ToggleInventory(int32 nWidthCount, int32 nHeightCount);
	void OpenInventory(int32 nWidthCount, int32 nHeightCount);

	void ItemDrag(bool bPickup);

private:
	/* flag whether inventory is open */
	bool m_bInvOpen;

	ACustomHUD * m_pHUD;
	Hotbar * m_pHotbar;

	FVector2D m_vInvStartpos;
	float m_fSlotSize;
	float m_fInventoryBorder;
	int32 m_nWidthCount;
	int32 m_nHeightCount;

	TArray<FVector2D> m_vaInvHitBoxPositions;

private:
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
