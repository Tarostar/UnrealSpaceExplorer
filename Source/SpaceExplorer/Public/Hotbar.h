

#pragma once

class ACustomHUD;

/**
 * Responsible for drawing the Hotbar
 */
class SPACEEXPLORER_API Hotbar
{
public:
	Hotbar();
	~Hotbar();

	void Init(ACustomHUD * pHUD);

	bool ShowHotbar();

	void DrawHotbar();

	void SetHotbarCount(int32 nCount);

	void UpdatePositions();

	FVector2D GetStartPos();

private:
	// number of hotbar slots
	int32 m_nHotbarCount;

	// flag whether hotbar is shown
	bool m_bShowHotbar;

	FVector2D m_vHotbarStartPos;

	// size of one hotbar slot
	float m_fHotbarSlotSize;

	TArray<FVector2D> m_vaHotbarHitBoxPositions;

	ACustomHUD * m_pHUD;

private:
	void SetStartPosition();
	void SetHitBoxPositionArray();
};
