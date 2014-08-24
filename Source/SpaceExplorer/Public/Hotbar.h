

#pragma once

class ACustomHUD;

#include "Hotbar.generated.h"

/**
 * Responsible for drawing the Hotbar
 */
UCLASS()
class SPACEEXPLORER_API AHotbar : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	/* Access to m_bShowHotbar */
	bool IsHotbarVisible();

	/* Setup Hotbar for use */
	void Init(ACustomHUD * pHUD, const FLinearColor& textColour, float fTextScale, float fSlotSize);

	/* Draw Hotbar when visible */
	void DrawHotbar();

	/* Set the number of hotbar slots (default: 5) */
	void SetSlotCount(int32 nCount);

	/* Update upper, left corner from where the hotbar is drawn and update hitbox position array (typically called when screen dimensions change) */
	void UpdatePositions();

	/* Get upper, left position of Hotbar (used to position other HUD elements relative to Hotbar) */
	FVector2D GetStartPos();

private:
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

	// texture background for hotbar
	UPROPERTY()
	UTexture *m_texture;

	// texture background for hotbar when selected
	UPROPERTY()
	UTexture *m_textureSelected;

	// colour of hotbar text
	UPROPERTY()
	FLinearColor m_textColour;

	// font of hotbar text
	UPROPERTY()
	UFont *m_font;

	// scale for text
	UPROPERTY()
	float m_fTextScale;

	// pointer to main HUD for access to AHUD methods, nCurrentRatio, etc.
	ACustomHUD * m_pHUD;

	/* Called by UpdatePositions */
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
