

#pragma once

class ACustomHUD;
class AInventoryObject;

#include "Hotbar.generated.h"

/**
 * Responsible for drawing the Hotbar
 */
UCLASS()
class SPACEEXPLORER_API AHotbar : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// colour of hotbar text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Hotbar)
	FLinearColor m_textColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Hotbar)
	FLinearColor m_textColourSelected;

	// texture background for hotbar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Hotbar)
	UTexture *m_texture;

	// texture background for hotbar when selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_Hotbar)
	UTexture *m_textureSelected;

	/* Access to m_bShowHotbar */
	bool IsHotbarVisible();

	/* Setup Hotbar for use */
	void Init(ACustomHUD * pHUD, float fTextScale, float fSlotSize, AInventoryObject* pHotbarObjects);

	/* Draw Hotbar when visible */
	void DrawHotbar();

	/* Set the number of hotbar slots (default: 5) */
	void SetSlotCount(int32 nCount);

	/* Update upper, left corner from where the hotbar is drawn and update hitbox position array (typically called when screen dimensions change) */
	void UpdatePositions();

	/* Get upper, left position of Hotbar (used to position other HUD elements relative to Hotbar) */
	FVector2D GetStartPos();

	bool ItemDrag(bool bPickup);
	bool CheckMouseOver(const FName BoxName, bool bBegin);

	void LinkHotbar(AInventoryObject* pHotbarObjects);

private:
	UPROPERTY()
	AInventoryObject* m_pHotbarObjects;

	// number of hotbar slots
	UPROPERTY()
	int32 m_nSlotCount;

	// flag whether hotbar is shown
	UPROPERTY()
	bool m_bShowHotbar;

	// upper, left position of hotbar
	UPROPERTY()
	FVector2D m_vHotbarStartPos;

	// size of one hotbar slot
	UPROPERTY()
	float m_fHotbarSlotSize;

	// upper, left coordinate of each hotbar hitbox (one for each hotbar slot)
	UPROPERTY()
	TArray<FVector2D> m_vaHotbarHitBoxPositions;

	// font of hotbar text
	UPROPERTY()
	UFont *m_font;

	// scale for text
	UPROPERTY()
	float m_fTextScale;

	// pointer to main HUD for access to AHUD methods, nCurrentRatio, etc.
	UPROPERTY()
	ACustomHUD * m_pHUD;

	UPROPERTY()
	int32 m_nHotbarHoverIndex;

	// hitbox name cursor is hovering over - for inventory will be a number indicating the inventory index
	UPROPERTY()
	FName m_cursorOverHitBoxName;

	// slot mouse cursor is hovering over
	UPROPERTY()
	int32 m_nHoverIndex;

	// index of item being dragged (we don't remove from inventory, but stop drawing it until reset, moved or dropped into world)
	UPROPERTY()
	int32 m_nDraggingItemIndex;

	/* Called by UpdatePositions */
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
