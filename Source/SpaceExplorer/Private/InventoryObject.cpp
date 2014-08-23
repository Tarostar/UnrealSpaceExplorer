

#include "SpaceExplorer.h"
#include "InventoryObject.h"


AInventoryObject::AInventoryObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	InvWidthCount = 10;
	InvHeightCount = 5;
}


