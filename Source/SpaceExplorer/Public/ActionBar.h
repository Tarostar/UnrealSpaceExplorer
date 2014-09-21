

#pragma once

#include "DragObject.h"
#include "CustomHUD.h"
#include "GameFramework/Actor.h"
#include "ActionBar.generated.h"

USTRUCT()
struct FActionStruct
{
	GENERATED_USTRUCT_BODY()

	class DragObject m_object;

	FVector2D m_hitBoxPosition;
};

/**
 * Responsible for drawing the ActionBar
 * Type of Action
 * (optional) Item Used - items have reference to action bar for when they are destroyed?
 */
UCLASS()
class SPACEEXPLORER_API AActionBar : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// colour of action bar text
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_ActionBar)
	FLinearColor m_textColour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_ActionBar)
	FLinearColor m_textColourSelected;

	// texture background for action bar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_ActionBar)
	UTexture *m_texture;

	// texture background for action bar when selected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD_ActionBar)
	UTexture *m_textureSelected;

	bool SetSlotAction(int32 nIndex, class DragObject& object);

	UFUNCTION(BlueprintCallable, Category = HUD_ActionBar)
	void DisableSlot(int32 nIndex);

	UFUNCTION(BlueprintCallable, Category = HUD_ActionBar)
	bool InvokeAction();

	bool LMBRelease();

	/* Access to m_bShowActionBar */
	bool IsVisible();

	/* Setup action bar for use */
	void Init(ACustomHUD * pHUD, float fTextScale, float fSlotSize);

	/* Draw action bar when visible */
	void Draw();

	/* Set the number of action bar slots (default: 5) */
	void SetSlotCount(int32 nCount);

	/* Update upper, left corner from where the action bar is drawn and update hitbox position array (typically called when screen dimensions change) */
	void UpdatePositions();

	/* Get upper, left position of action bar (used to position other HUD elements relative to action bar) */
	FVector2D GetStartPos();

	bool DragDrop(bool bDelete, class DragObject& item);
	bool CheckMouseOver(const FName BoxName, bool bBegin);

private:
	// flag whether action bar is shown
	UPROPERTY()
	bool m_bShowActionBar;

	// upper, left position of action bar
	UPROPERTY()
	FVector2D m_vStartPos;

	// size of one action bar slot
	UPROPERTY()
	float m_fSlotSize;

	// upper, left coordinate of each action bar hitbox (one for each action bar slot)
	//UPROPERTY()
	TArray<FActionStruct> m_actionSlots;

	// font of action bar text
	UPROPERTY()
	UFont *m_font;

	// scale for text
	UPROPERTY()
	float m_fTextScale;

	// pointer to main HUD for access to AHUD methods, nCurrentRatio, etc.
	UPROPERTY()
	ACustomHUD * m_pHUD;

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
