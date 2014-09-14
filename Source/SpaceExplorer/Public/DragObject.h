

#pragma once

#include "Enums.h"

/**
 * 
 */
class SPACEEXPLORER_API DragObject
{
public:
	DragObject();
	~DragObject();

	// start dragging
	void Init(const FString& label, const EActionType::Type& type, const int32& nInventoryID, const int32& nSlotIndex);

	// stop dragging
	void Drop();
	
	// getters
	bool IsDragging();
	FString GetLabel();
	EActionType::Type GetType();
	int32 GetSlotIndex();
	int32 GetInventoryID();

private:
	bool m_bDragging;

	// (optional) label (used for actions)
	FString	m_strLabel;

	// (optional) type of action, e.g. use for items
	EActionType::Type m_nType;

	// (optional: -1 if not used) unique ID of inventory for character/pawn
	int32 m_nInventoryID;

	// (optional) only valid if m_nSourceInventory is set and then slot index in source inventory
	int32 m_nSlotIndex;
};
