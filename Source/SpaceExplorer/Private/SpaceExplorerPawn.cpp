// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
#include "Net/UnrealNetwork.h"
#include "UsableObject.h"
#include "CustomHUD.h"
#include "InventoryObject.h"
#include "SpaceExplorerPawn.h"

ASpaceExplorerPawn::ASpaceExplorerPawn(const class FPostConstructInitializeProperties& PCIP)
: Super(PCIP)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> planeMesh;
		FConstructorStatics() : planeMesh(TEXT("/Game/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	m_planeMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PlaneMesh0"));
	m_planeMesh->SetStaticMesh(ConstructorStatics.planeMesh.Get());
	RootComponent = m_planeMesh;

	// Create a spring arm component
	m_fMinimumArmTargetDistance = 160.0f;
	m_springArm = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("SpringArm0"));
	m_springArm->AttachTo(RootComponent);
	m_springArm->TargetArmLength = m_fMinimumArmTargetDistance; // The camera follows at this distance behind the character	
	m_springArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	m_springArm->bEnableCameraLag = false;
	m_springArm->bDoCollisionTest = false;
	m_springArm->CameraLagSpeed = 0.0f;
	m_springArm->bInheritRoll = false;
	//m_springArm->bInheritPitch = false;
	//m_springArm->bInheritYaw = false;

	// Create camera component 
	m_camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera0"));
	m_camera->AttachTo(m_springArm, USpringArmComponent::SocketName);
	m_camera->bUseControllerViewRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	m_fAcceleration = 1000.f;
	m_fTurnSpeed = 100.f;
	m_fMaxSpeed = 8000.f;
	m_fMinSpeed = 0.f;
	m_fCurrentForwardSpeed = 0.f;

	CurrentZoom = 1.f;
	m_bFreeMouseLook = false;

	m_fCurrentDamage = 0.f;
	m_fMaxDamage = 8000.f;

	bool m_bFirstPersonView = false;

	m_fMaxUseDistance = 800;
	m_bHasNewFocus = true;

}

void ASpaceExplorerPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ASpaceExplorerPawn, m_inventoryObjects);
}


void ASpaceExplorerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		//AInventoryObject * invObject = World->SpawnActor<AInventoryObject>(SpawnParams);
		AInventoryObject * invObject = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());
		
		if (invObject)
		{
			invObject->Init(AssignUniqueInventoryID(), 5, 5);
			invObject->m_inventorySlots.SetNum(invObject->m_nInvHeightCount * invObject->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject);
		}

		AInventoryObject * invObject2 = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());

		if (invObject2)
		{
			invObject2->Init(AssignUniqueInventoryID(), 10, 10);
			invObject2->m_inventorySlots.SetNum(invObject2->m_nInvHeightCount * invObject2->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject2);
		}

		AInventoryObject * invObject3 = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());

		if (invObject3)
		{
			invObject3->Init(AssignUniqueInventoryID(), 10, 5);
			invObject3->m_inventorySlots.SetNum(invObject3->m_nInvHeightCount * invObject3->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject3);
		}	
	}

}

void ASpaceExplorerPawn::OnConstruction(const FTransform& Transform)
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, TEXT("Default Spring Rot: ") + TargetSpringRotation.ToString());

}

void ASpaceExplorerPawn::ToggleFirstPerson()
{
	if (m_bFirstPersonView)
	{
		// toggle back to springarm
		m_camera->AttachTo(m_springArm, USpringArmComponent::SocketName);
		m_camera->RelativeLocation = FVector(0, 0, 0);
		m_planeMesh->SetVisibility(true);
	}
	else
	{
		// toggle to first person (better to use socket for cockpit and do relative to that or at least get the location at the tip of the ship...)
		m_camera->AttachTo(m_planeMesh);
		m_camera->RelativeLocation = FVector(100.0f, 0, 00.0f);

		// cheating...
		m_planeMesh->SetVisibility(false);
		//m_planeMesh->SetHiddenInGame
	}

	m_bFirstPersonView = !m_bFirstPersonView;

	SetMouseLook(m_bFirstPersonView);
}

void ASpaceExplorerPawn::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	const FVector LocalMove = FVector(m_fCurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator fDeltaRotation(0, 0, 0);
	fDeltaRotation.Pitch = m_fCurrentPitchSpeed * DeltaSeconds;
	fDeltaRotation.Yaw = m_fCurrentYawSpeed * DeltaSeconds;
	fDeltaRotation.Roll = m_fCurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(fDeltaRotation);

	if (!m_bFreeMouseLook && !m_bFirstPersonView && !m_springArm->bUseControllerViewRotation)
	{
		// user is not rotating camera in third person view, smoothly interpolate to target position

		FRotator currentSpringRotation = m_springArm->GetComponentRotation();
		FRotator targetRot = GetActorRotation();

		// set spring arm back
		if (!targetRot.Equals(currentSpringRotation, 0.000001f))
		{
			m_springArm->SetWorldRotation(FMath::RInterpTo(currentSpringRotation, targetRot, GetWorld()->GetDeltaSeconds(), 2.f));
		}
	}
	else if (!m_bFreeMouseLook && m_bFirstPersonView && !m_camera->bUseControllerViewRotation)
	{
		// user is not rotating camera in first person view, smoothly interpolate to target position

		FRotator currentCameraRotation = m_camera->GetComponentRotation();
		FRotator targetRot = GetActorRotation();

		// set camera back
		if (!targetRot.Equals(currentCameraRotation, 0.000001f))
		{
			m_camera->SetWorldRotation(FMath::RInterpTo(currentCameraRotation, targetRot, GetWorld()->GetDeltaSeconds(), 2.f));
		}
	}

	// HandleUsableObject();
}

/*void ASpaceExplorerPawn::HandleUsableObject()
{
	// Update actor currently being looked at by player.
	if (Controller && Controller->IsLocalController())
	{
		AUsableObject* usable = GetUsableInView();

		// End Focus
		if (FocusedUsableObject != usable)
		{
			if (FocusedUsableObject)
			{
				FocusedUsableObject->EndFocusItem();
			}

			m_bHasNewFocus = true;
		}

		// Assign new Focus
		FocusedUsableObject = usable;

		// Start Focus.
		if (usable)
		{
			if (m_bHasNewFocus)
			{
				usable->StartFocusItem();
				m_bHasNewFocus = false;
			}
		}
	}
}*/

void ASpaceExplorerPawn::ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	m_fCurrentForwardSpeed = 0.f;
}


void ASpaceExplorerPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// axes
	InputComponent->BindAxis("Thrust", this, &ASpaceExplorerPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &ASpaceExplorerPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &ASpaceExplorerPawn::MoveRightInput);
	InputComponent->BindAxis("Turn", this, &ASpaceExplorerPawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ASpaceExplorerPawn::AddControllerPitchInput);
	InputComponent->BindAxis("Zoom", this, &ASpaceExplorerPawn::Zoom);

	// actions
	InputComponent->BindAction("ResetCamera", IE_Pressed, this, &ASpaceExplorerPawn::CamReset);
	InputComponent->BindAction("MouseLook", IE_Pressed, this, &ASpaceExplorerPawn::StartMouseLook);
	InputComponent->BindAction("MouseLook", IE_Released, this, &ASpaceExplorerPawn::StopMouseLook);
	InputComponent->BindAction("CameraFollow", IE_Pressed, this, &ASpaceExplorerPawn::ToggleFreeMouseLook);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ASpaceExplorerPawn::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ASpaceExplorerPawn::ZoomOut);
	InputComponent->BindAction("ToggleFirstPerson", IE_Pressed, this, &ASpaceExplorerPawn::ToggleFirstPerson);

	InputComponent->BindAction("ToggleAllInventory", IE_Pressed, this, &ASpaceExplorerPawn::ToggleAllInventory);
	InputComponent->BindAction("ToggleInventoryOne", IE_Pressed, this, &ASpaceExplorerPawn::ToggleInventoryOne);
	InputComponent->BindAction("ToggleInventoryTwo", IE_Pressed, this, &ASpaceExplorerPawn::ToggleInventoryTwo);
	InputComponent->BindAction("ToggleInventoryThree", IE_Pressed, this, &ASpaceExplorerPawn::ToggleInventoryThree);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ASpaceExplorerPawn::Interact);
	InputComponent->BindAction("Delete", IE_Pressed, this, &ASpaceExplorerPawn::Delete);

	// TODO: this should be interact, pick up or similar instead
	InputComponent->BindAction("LMButton", IE_Pressed, this, &ASpaceExplorerPawn::LMBPressed);
	InputComponent->BindAction("LMButton", IE_Released, this, &ASpaceExplorerPawn::LMBRelease);
}

void ASpaceExplorerPawn::ZoomIn()
{
	Zoom(-1.0f);
}

void ASpaceExplorerPawn::ZoomOut()
{
	Zoom(1.0f);
}

void ASpaceExplorerPawn::Fire()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Bang!"));
}

void ASpaceExplorerPawn::ToggleFreeMouseLook()
{
	m_bFreeMouseLook = !m_bFreeMouseLook;

	SetMouseLook(m_bFreeMouseLook);
}

void ASpaceExplorerPawn::CamReset()
{
	// set controller rotation (for when resuming camera look control)

	// m_springArm->SetWorldRotation(GetActorRotation());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Rot: ") + TargetSpringRotation.ToString());
}

void ASpaceExplorerPawn::StartMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (m_bFreeMouseLook)
		return;

	SetMouseLook(true);
}

void ASpaceExplorerPawn::SetMouseLook(bool bMouseLook)
{
	// rotate the camera or arm based on the controller
	if (m_bFirstPersonView)
	{
		// set camera to current arm position
		ClientSetRotation(m_camera->GetComponentRotation());
		m_camera->bUseControllerViewRotation = bMouseLook;
	}
	else
	{
		// set controller to current arm position
		ClientSetRotation(m_springArm->GetComponentRotation());
		m_springArm->bUseControllerViewRotation = bMouseLook;
	}
}

void ASpaceExplorerPawn::StopMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (m_bFreeMouseLook)
		return;

	SetMouseLook(false);
}

void ASpaceExplorerPawn::Zoom(float Val)
{
	if (FMath::IsNearlyEqual(Val, 0.f))
		return; // nothing going on...

	if (Val < 0.f && CurrentZoom <= 1.0f)
		return; // can't zoom in further

	if (Val > 0.f && CurrentZoom >= 3.0f)
		return; // can't zoom out further

	CurrentZoom += Val;

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Zoom: ") + FString::SanitizeFloat(Val));

}

void ASpaceExplorerPawn::AddControllerYawInput(float Val)
{
	if (m_springArm->bUseControllerViewRotation || m_camera->bUseControllerViewRotation)
		Super::AddControllerYawInput(Val);
}

void ASpaceExplorerPawn::AddControllerPitchInput(float Val)
{
	if (m_springArm->bUseControllerViewRotation || m_camera->bUseControllerViewRotation)
		Super::AddControllerPitchInput(Val);
}


void ASpaceExplorerPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// acceleration
	float CurrentAcc = bHasInput ? (Val * m_fAcceleration) : 0.0f;
	// Calculate new speed
	float NewForwardSpeed = m_fCurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	m_fCurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, m_fMinSpeed, m_fMaxSpeed);

	//m_springArm->TargetArmLength = bHasInput ? m_fMinimumArmTargetDistance * (2 - m_fCurrentForwardSpeed / (m_fMaxSpeed - m_fMinSpeed)) : MaximumArmTargetDistance * (2 - m_fCurrentForwardSpeed / (m_fMaxSpeed - m_fMinSpeed));
	m_springArm->TargetArmLength = m_fMinimumArmTargetDistance * CurrentZoom * (2 - m_fCurrentForwardSpeed / (m_fMaxSpeed - m_fMinSpeed));
}

void ASpaceExplorerPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float m_fTargetPitchSpeed = (Val * m_fTurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	m_fTargetPitchSpeed += (FMath::Abs(m_fCurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	m_fCurrentPitchSpeed = FMath::FInterpTo(m_fCurrentPitchSpeed, m_fTargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ASpaceExplorerPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * m_fTurnSpeed);

	// Smoothly interpolate to target yaw speed
	m_fCurrentYawSpeed = FMath::FInterpTo(m_fCurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (m_fCurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	m_fCurrentRollSpeed = FMath::FInterpTo(m_fCurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

/*
Performs raytrace to find closest looked-at UsableObject.
*/
/*AUsableObject* ASpaceExplorerPawn::GetUsableInView()
{
	
	FVector camLoc;
	FRotator camRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(camLoc, camRot);
	const FVector start_trace = camLoc;
	const FVector direction = camRot.Vector();
	const FVector end_trace = start_trace + (direction * m_fMaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, start_trace, end_trace, COLLISION_PROJECTILE, TraceParams);

	return Cast<AUsableObject>(Hit.GetActor());
}

void ASpaceExplorerPawn::Use_Implementation()
{
	// enable “replicates” property in your Blueprint.

	AUsableObject* usable = GetUsableInView();
	if (usable)
	{
		usable->OnUsed(this);
	}
}

bool ASpaceExplorerPawn::Use_Validate()
{
	// No special server-side validation performed.
	return true;
}*/

void ASpaceExplorerPawn::ToggleAllInventory()
{
	if (!Controller)
	{
		return;
	}

	APlayerController* pc = Cast<APlayerController>(Controller);
	if (!pc)
	{
		return;
	}

	ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
	if (pHUD)
	{
		pHUD->ToggleAllInventory();
	}
}

bool ASpaceExplorerPawn::ToggleInventory(int32 nIndex)
{
	if (!Controller || !m_inventoryObjects.IsValidIndex(nIndex))
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryOne Failed because missing controller or index is invalid"));
		return false;
	}

	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc == nullptr)
	{
		return false;
	}

	ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
	if (pHUD == nullptr)
	{
		return false;
	}

	pHUD->ToggleInventory(m_inventoryObjects[nIndex]->GetID());
	return true;
}

void ASpaceExplorerPawn::ToggleInventoryOne()
{
	ToggleInventory(0);
}

void ASpaceExplorerPawn::ToggleInventoryTwo()
{
	ToggleInventory(1);
}

void ASpaceExplorerPawn::ToggleInventoryThree()
{
	ToggleInventory(2);
}

bool ASpaceExplorerPawn::AddItem(AUsableObject * pItem)
{
	for (int i = 0; i < m_inventoryObjects.Num(); i++)
	{
		if (m_inventoryObjects[i]->AddItemFirstAvailableSlot(pItem))
		{
			// successfully added item
			return true;
		}
	}

	// did not find any suitable inventory space
	return false;
}

int32 ASpaceExplorerPawn::AssignUniqueInventoryID()
{
	// assign unique ID
	int32 nUniqueID = 0;
	bool bExists = true;
	while (bExists)
	{
		// start by assuming this unique ID does not exist
		bExists = false;

		// check if it does exists
		for (int i = 0; i < m_inventoryObjects.Num(); i++)
		{
			if (m_inventoryObjects[i]->GetID() == nUniqueID)
			{
				// found ID in use - try next
				bExists = true;
				nUniqueID++;
				break;
			}
		}
	}

	return nUniqueID;
}

int32 ASpaceExplorerPawn::GetInventoryIndexFromID(int32 nID)
{
	for (int i = 0; i < m_inventoryObjects.Num(); i++)
	{
		if (m_inventoryObjects[i]->GetID() == nID)
		{
			// found ID
			return i;
		}
	}

	// does not exist
	return -1;
}

AInventoryObject* ASpaceExplorerPawn::GetInventoryObjectFromIndex(int32 nIndex)
{
	if (m_inventoryObjects.IsValidIndex(nIndex))
	{
		return m_inventoryObjects[nIndex];
	}

	return nullptr;
}

AInventoryObject* ASpaceExplorerPawn::GetInventoryObjectFromID(int32 nID)
{
	for (int i = 0; i < m_inventoryObjects.Num(); i++)
	{
		if (m_inventoryObjects[i]->GetID() == nID)
		{
			// found ID
			return m_inventoryObjects[i];
		}
	}

	// does not exist
	return nullptr;
}

int32 ASpaceExplorerPawn::GetInventoryObjectCount()
{
	return m_inventoryObjects.Num();
}

void ASpaceExplorerPawn::Delete()
{
	if (!Controller)
	{
		return;
	}

	APlayerController* pc = Cast<APlayerController>(Controller);
	if (!pc)
	{
		return;
	}

	ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
	if (pHUD)
	{
		pHUD->Delete();
	}
}

void ASpaceExplorerPawn::InvokeAction(class DragObject& item)
{
	switch (item.GetType())
	{
		case EActionType::Use: UseItem(item.GetInventoryID(), item.GetSlotIndex());
			return;
		case EActionType::Attack: Fire();
			return;
	}
}

void ASpaceExplorerPawn::UseItem_Implementation(int32 nInventoryID, int32 nSlotIndex)
{
	// TODO: does this work in multiplayer

	// enable “replicates” property in blueprints.

	AInventoryObject* pInventoryObject = GetInventoryObjectFromID(nInventoryID);
	if (pInventoryObject == nullptr)
	{
		return;
	}

	AUsableObject* pObject = pInventoryObject->GetItem(nSlotIndex);
	if (pObject == nullptr)
	{
		return;
	}

	pObject->InvokeAction();

	pObject->OnUsed(this);
}

bool ASpaceExplorerPawn::UseItem_Validate(int32 nInventoryID, int32 nSlotIndex)
{
	// No special server-side validation performed.
	return true;
}

void ASpaceExplorerPawn::Interact()
{
	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc == nullptr)
	{
		return;
	}

	ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
	if (pHUD)
	{
		if (pHUD->Interact())
		{
			// interacted with HUD item
			return;
		}
	}

	// interact with world

	// TODO: implement
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Interacting with the world."));
}

void ASpaceExplorerPawn::LMBPressed()
{
	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
		if (pHUD)
		{
			if (pHUD->LMBPressed())
			{
				// interacted with HUD item
				return;
			}
		}
	}

	// interact with world

	Fire();
}

void ASpaceExplorerPawn::LMBRelease()
{
	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
		if (pHUD)
		{
			if (pHUD->LMBRelease())
			{
				// interacted with HUD item
				return;
			}
		}
	}

	// interact with world
}
