

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
	// texture background
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_texture;

	// texture background when selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	UTexture *m_textureSelected;

	/* flag whether inventory is open */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Inventory)
	bool m_bInvOpen;

	void Init(ACustomHUD * pHUD, AActionBar * pActionBar, float fSlotSize = 64.f, float fInventoryBorder = 0.f);

	bool IsInvOpen();

	void DrawInventory();

	void UpdatePositions();

	void ToggleInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory);
	void CloseInventory();
	void OpenInventory(AInventoryObject* pInventory, bool bInGroup, AInventory* pPreviousInventory);
	void Move(bool bInGroup, AInventory* pPreviousInventory);
	
	int32 GetID();

	bool ItemDrag(bool bPickup, class DragObject& item);
	bool CheckMouseOver(const FName BoxName, bool bBegin);

	bool InvokeAction();
	bool ActiveHitbox();

private:
	UPROPERTY()
	ACustomHUD * m_pHUD;

	// pointer to action bar for relative positioning when action bar is visible
	// TODO: adding UProperty here causes CustomHUD to fail compiling?!?!?...?
	//UPROPERTY()
	AActionBar * m_ActionBar;

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
	bool m_bInGroup;

	UPROPERTY()
	AInventoryObject* m_pInventory;

	// optional pointer to previously drawn inventory for calculating position
	UPROPERTY()
	AInventory* m_pPreviousInventory;

private:
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
