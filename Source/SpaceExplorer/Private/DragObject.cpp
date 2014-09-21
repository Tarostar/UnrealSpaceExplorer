

#include "SpaceExplorer.h"
#include "DragObject.h"

DragObject::DragObject()
{
	m_bDragging = false;
	m_strLabel = "";
	m_nInventoryID = -1;
	m_nSlotIndex = -1;
	m_nType = EActionType::Use;
}

DragObject::~DragObject()
{
}

void DragObject::Init(const FString& label, const EActionType::Type& type, const int32& nInventoryID, const int32& nSlotIndex, bool bDragging)
{
	m_bDragging = bDragging;

	m_strLabel = label;

	m_nType = type;

	m_nInventoryID = nInventoryID;

	m_nSlotIndex = nSlotIndex;
}

bool DragObject::IsDragging()
{
	return m_bDragging;
}

void DragObject::Drop()
{
	m_bDragging = false;
	m_strLabel = "";
	m_nInventoryID = -1;
	m_nSlotIndex = -1;
	m_nType = EActionType::Use;
}

FString DragObject::GetLabel()
{
	return m_strLabel;
}

EActionType::Type DragObject::GetType()
{
	return m_nType;
}

int32 DragObject::GetSlotIndex()
{
	return m_nSlotIndex;
}

int32 DragObject::GetInventoryID()
{
	return m_nInventoryID;
}
