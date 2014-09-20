

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

bool DragObject::InvokeAction()
{
	switch (m_nType)
	{
		case EActionType::Use :
			if (m_nInventoryID >= 0 && m_nSlotIndex >= 0)
			{
				// TODO: interface perhaps to trigger a generic item action - need to think through the whole invoke actions structure
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("TODO: implement."));
				return true;
			}
	}

	// action type undefined
	return false; 
}
