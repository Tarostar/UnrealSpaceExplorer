
#include "SpaceExplorer.h"
#include "Net/UnrealNetwork.h"
#include "UsableObject.h"


AUsableObject::AUsableObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_name = "none";
	m_nInvWidth = 1;
	m_nInvHeight = 1;
	m_bIsStackable = false;
	m_nCount = 1;
	
	// TODO: proper default texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> ItemTextureSelOb(TEXT("/Game/Textures/redThing_128_transparent"));
	m_inventoryTexture = ItemTextureSelOb.Object;

	// TODO: review
	m_mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	RootComponent = m_mesh;

	m_worldOwner = NULL;
}

void AUsableObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AUsableObject, m_name);
	DOREPLIFETIME(AUsableObject, m_nInvWidth);
	DOREPLIFETIME(AUsableObject, m_nInvHeight);
	DOREPLIFETIME(AUsableObject, m_bIsStackable);
	DOREPLIFETIME(AUsableObject, m_nCount);
}


void AUsableObject::InitMesh()
{
	// TODO: is this how we want to do it?
	m_mesh->SetSimulatePhysics(true);
	m_mesh->WakeRigidBody();
}

void AUsableObject::Collect(AActor * pNewOwner)
{
	// grab item and put in inventory (unregister component)
	
	if (m_mesh)
	{
		// https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html

		m_worldOwner = GetOwner();
		SetOwner(pNewOwner);
		m_mesh->UnregisterComponent(); // this should hide mesh - otherwise might try DestroyComponent
	}
}

void AUsableObject::Drop()
{
	// put item back into world (register component and set location in world)

	// TODO: transform?

	if (m_mesh)
	{
		// https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html

		// TODO: this variable is just temporary until I can figure out how to set it back to correct "world owner"
		SetOwner(m_worldOwner);

		// TODO: this is just a quick and dirty method to set a transform/location - needs work
		FTransform transform = GetOwner()->GetTransform();
		FVector vector = transform.GetTranslation();
		vector.Z += 10.f;
		transform.SetTranslation(vector);
		SetActorTransform(transform);
		m_mesh->RegisterComponent();
	}
}

bool AUsableObject::IsVisibleInWorld()
{
	return m_mesh->IsRegistered();
}
