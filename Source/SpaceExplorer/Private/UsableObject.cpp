

#include "SpaceExplorer.h"
#include "UsableObject.h"


AUsableObject::AUsableObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_nInvWidth = 1;
	m_nInvHeight = 1;
}


