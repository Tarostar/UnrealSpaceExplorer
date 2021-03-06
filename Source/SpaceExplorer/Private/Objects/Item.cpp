
#include "SpaceExplorer.h"
#include "Item.h"
#include "Net/UnrealNetwork.h"

AItem::AItem(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	m_nInvWidth = 1;
	m_nInvHeight = 1;
	m_bIsStackable = false;
	m_nCount = 1;

	// ideally this should generate a unique identifier
	// m_ID = -1;
	
	// TODO: proper default texture placeholder (texture should always be set in blueprints)
	static ConstructorHelpers::FObjectFinder<UTexture2D> ItemTextureSelOb(TEXT("/Game/Textures/redThing_128_transparent"));
	m_inventoryTexture = ItemTextureSelOb.Object;

	// TODO: review
	m_mesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mesh"));
	RootComponent = m_mesh;

	m_worldOwner = nullptr;

	bReplicates = true;
}

void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AItem, m_nInvWidth);
	DOREPLIFETIME(AItem, m_nInvHeight);
	DOREPLIFETIME(AItem, m_bIsStackable);
	DOREPLIFETIME(AItem, m_nCount);
}


void AItem::InitMesh()
{
	// TODO: is this how we want to do it?
	m_mesh->SetSimulatePhysics(true);
	m_mesh->WakeRigidBody();
}

void AItem::Collect(AActor * pNewOwner)
{
	// grab item and put in inventory (unregister component)
	
	if (m_mesh)
	{
		// https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html

		// TODO: seems this returns nullptr... so it has no world owner? Need to learn more about owner
		m_worldOwner = GetOwner();
		SetOwner(pNewOwner);
		m_mesh->UnregisterComponent(); // this should hide mesh - otherwise might try DestroyComponent
	}
}

void AItem::Drop()
{
	// put item back into world (register component and set location in world)

	if (m_mesh)
	{
		// https://docs.unrealengine.com/latest/INT/Programming/UnrealArchitecture/Actors/Components/index.html

		// get item owner (should be player)
		AActor* owner = GetOwner();

		if (owner)
		{
			// TODO: this is just a quick and dirty method to set a transform/location - needs work
			FTransform transform = owner->GetTransform();
			FVector vector = transform.GetTranslation();
			vector.Z += 10.f;
			transform.SetTranslation(vector);
			SetActorTransform(transform);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("no world owner."));
		}

		// TODO: this variable is just temporary until I can figure out how to set it back to correct "world owner"
		SetOwner(m_worldOwner);

		m_mesh->RegisterComponent();
	}
}

bool AItem::IsVisibleInWorld()
{
	return m_mesh->IsRegistered();
}


bool AItem::InvokeAction()
{
	// this should most likely be just a baseclass placeholder and then children of usable object implement the actual action invoked
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Action invoked on usable object."));

	return true;
}

int32 AItem::GetItemWidth()
{
	return m_nInvWidth;
}

int32 AItem::GetItemHeight()
{
	return m_nInvHeight;
}

void AItem::SaveLoad(FArchive& Ar)
{
	Super::SaveLoad(Ar);

	// remember m_worldOwner (AActor)?
	Ar << m_nInvWidth;
	Ar << m_nInvHeight;
	Ar << m_bIsStackable;
	Ar << m_nCount;
	// we need a way to retrieve relevant texture and mesh...
	//Ar << m_inventoryTexture;

}
