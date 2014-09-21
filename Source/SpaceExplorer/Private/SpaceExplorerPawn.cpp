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
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

	// Create a spring arm component
	MinimumArmTargetDistance = 160.0f;
	SpringArm = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = MinimumArmTargetDistance; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->bDoCollisionTest = false;
	SpringArm->CameraLagSpeed = 0.0f;
	SpringArm->bInheritRoll = false;
	//SpringArm->bInheritPitch = false;
	//SpringArm->bInheritYaw = false;

	// Create camera component 
	Camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUseControllerViewRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 1000.f;
	TurnSpeed = 100.f;
	MaxSpeed = 8000.f;
	MinSpeed = 0.f;
	CurrentForwardSpeed = 0.f;

	CurrentZoom = 1.f;
	bFreeMouseLook = false;

	CurrentDamage = 0.f;
	MaxDamage = MaxSpeed;

	bool bFirstPersonView = false;

	MaxUseDistance = 800;
	bHasNewFocus = true;

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
			invObject2->Init(AssignUniqueInventoryID(), 10, 2);
			invObject2->m_inventorySlots.SetNum(invObject2->m_nInvHeightCount * invObject2->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject2);
		}



		/*m_hotbarObjects = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());

		if (m_hotbarObjects)
		{
			m_hotbarObjects->m_nInvHeightCount = 1;
			m_hotbarObjects->m_nInvWidthCount = 5;
			m_hotbarObjects->m_inventorySlots.SetNum(m_hotbarObjects->m_nInvHeightCount * m_hotbarObjects->m_nInvWidthCount);
		}*/		
	}

}

void ASpaceExplorerPawn::OnConstruction(const FTransform& Transform)
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, TEXT("Default Spring Rot: ") + TargetSpringRotation.ToString());

}

void ASpaceExplorerPawn::ToggleFirstPerson()
{
	if (bFirstPersonView)
	{
		// toggle back to springarm
		Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
		Camera->RelativeLocation = FVector(0, 0, 0);
		PlaneMesh->SetVisibility(true);
	}
	else
	{
		// toggle to first person (better to use socket for cockpit and do relative to that or at least get the location at the tip of the ship...)
		Camera->AttachTo(PlaneMesh);
		Camera->RelativeLocation = FVector(100.0f, 0, 00.0f);

		// cheating...
		PlaneMesh->SetVisibility(false);
		//PlaneMesh->SetHiddenInGame
	}

	bFirstPersonView = !bFirstPersonView;

	SetMouseLook(bFirstPersonView);
}

void ASpaceExplorerPawn::Tick(float DeltaSeconds)
{
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);

	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	if (!bFreeMouseLook && !bFirstPersonView && !SpringArm->bUseControllerViewRotation)
	{
		// user is not rotating camera in third person view, smoothly interpolate to target position

		FRotator CurrentSpringRotation = SpringArm->GetComponentRotation();
		FRotator TargetRot = GetActorRotation();

		// set spring arm back
		if (!TargetRot.Equals(CurrentSpringRotation, 0.000001f))
		{
			SpringArm->SetWorldRotation(FMath::RInterpTo(CurrentSpringRotation, TargetRot, GetWorld()->GetDeltaSeconds(), 2.f));
		}
	}
	else if (!bFreeMouseLook && bFirstPersonView && !Camera->bUseControllerViewRotation)
	{
		// user is not rotating camera in first person view, smoothly interpolate to target position

		FRotator CurrentCameraRotation = Camera->GetComponentRotation();
		FRotator TargetRot = GetActorRotation();

		// set camera back
		if (!TargetRot.Equals(CurrentCameraRotation, 0.000001f))
		{
			Camera->SetWorldRotation(FMath::RInterpTo(CurrentCameraRotation, TargetRot, GetWorld()->GetDeltaSeconds(), 2.f));
		}
	}

	HandleUsableObject();
}

void ASpaceExplorerPawn::HandleUsableObject()
{
	// Update actor currently being looked at by player.
	/*if (Controller && Controller->IsLocalController())
	{
		AUsableObject* usable = GetUsableInView();

		// End Focus
		if (FocusedUsableObject != usable)
		{
			if (FocusedUsableObject)
			{
				FocusedUsableObject->EndFocusItem();
			}

			bHasNewFocus = true;
		}

		// Assign new Focus
		FocusedUsableObject = usable;

		// Start Focus.
		if (usable)
		{
			if (bHasNewFocus)
			{
				usable->StartFocusItem();
				bHasNewFocus = false;
			}
		}
	}*/
}

void ASpaceExplorerPawn::ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	CurrentForwardSpeed = 0.f;
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
	bFreeMouseLook = !bFreeMouseLook;

	SetMouseLook(bFreeMouseLook);
}

void ASpaceExplorerPawn::CamReset()
{
	// set controller rotation (for when resuming camera look control)

	// SpringArm->SetWorldRotation(GetActorRotation());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Rot: ") + TargetSpringRotation.ToString());
}

void ASpaceExplorerPawn::StartMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (bFreeMouseLook)
		return;

	SetMouseLook(true);
}

void ASpaceExplorerPawn::SetMouseLook(bool bMouseLook)
{
	// rotate the camera or arm based on the controller
	if (bFirstPersonView)
	{
		// set camera to current arm position
		ClientSetRotation(Camera->GetComponentRotation());
		Camera->bUseControllerViewRotation = bMouseLook;
	}
	else
	{
		// set controller to current arm position
		ClientSetRotation(SpringArm->GetComponentRotation());
		SpringArm->bUseControllerViewRotation = bMouseLook;
	}
}

void ASpaceExplorerPawn::StopMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (bFreeMouseLook)
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
	if (SpringArm->bUseControllerViewRotation || Camera->bUseControllerViewRotation)
		Super::AddControllerYawInput(Val);
}

void ASpaceExplorerPawn::AddControllerPitchInput(float Val)
{
	if (SpringArm->bUseControllerViewRotation || Camera->bUseControllerViewRotation)
		Super::AddControllerPitchInput(Val);
}


void ASpaceExplorerPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// acceleration
	float CurrentAcc = bHasInput ? (Val * Acceleration) : 0.0f;//(-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);

	//SpringArm->TargetArmLength = bHasInput ? MinimumArmTargetDistance * (2 - CurrentForwardSpeed / (MaxSpeed - MinSpeed)) : MaximumArmTargetDistance * (2 - CurrentForwardSpeed / (MaxSpeed - MinSpeed));
	SpringArm->TargetArmLength = MinimumArmTargetDistance * CurrentZoom * (2 - CurrentForwardSpeed / (MaxSpeed - MinSpeed));

	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("Distance: ") + FString::SanitizeFloat(SpringArm->TargetArmLength));
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("Speed Mod: ") + FString::SanitizeFloat(2 - CurrentForwardSpeed / (MaxSpeed - MinSpeed)));
}

void ASpaceExplorerPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ASpaceExplorerPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

/*
Performs raytrace to find closest looked-at UsableObject.
*/
AUsableObject* ASpaceExplorerPawn::GetUsableInView()
{
	
	FVector camLoc;
	FRotator camRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(camLoc, camRot);
	const FVector start_trace = camLoc;
	const FVector direction = camRot.Vector();
	const FVector end_trace = start_trace + (direction * MaxUseDistance);

	FCollisionQueryParams TraceParams(FName(TEXT("")), true, this);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, start_trace, end_trace, COLLISION_PROJECTILE, TraceParams);

	return Cast<AUsableObject>(Hit.GetActor());
}

/*
Runs on Server. Perform "OnUsed" on currently viewed UsableObject if implemented.
*/
void ASpaceExplorerPawn::Use_Implementation()
{
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
}

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

void ASpaceExplorerPawn::ToggleInventoryOne()
{
	if (!Controller || !m_inventoryObjects.IsValidIndex(0))
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryOne Failed because missing controller or index is invalid"));
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
		pHUD->ToggleInventory(m_inventoryObjects[0]->GetID());
	}
}

void ASpaceExplorerPawn::ToggleInventoryTwo()
{
	if (!Controller || !m_inventoryObjects.IsValidIndex(1))
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryTwo Failed because missing controller or index is invalid"));
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
		pHUD->ToggleInventory(m_inventoryObjects[1]->GetID());
	}
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
		case EActionType::Use: UseItem(item);
			return;
		case EActionType::Attack: Fire();
			return;
	}
}

void ASpaceExplorerPawn::UseItem(class DragObject& item)
{
	// TODO: because of server replication implementation should use virtual void Use() function

	AInventoryObject* pInventoryObject = GetInventoryObjectFromID(item.GetInventoryID());
	if (pInventoryObject == nullptr)
	{
		return;
	}

	AUsableObject* pObject = pInventoryObject->GetItem(item.GetSlotIndex());
	if (pObject == nullptr)
	{
		return;
	}

	pObject->InvokeAction();
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
	Fire();
}

void ASpaceExplorerPawn::LMBRelease()
{
	APlayerController* pc = Cast<APlayerController>(Controller);
	if (pc == nullptr)
	{
		return;
	}

	ACustomHUD* pHUD = Cast<ACustomHUD>(pc->GetHUD());
	if (pHUD)
	{
		if (pHUD->LMBRelease())
		{
			// interacted with HUD item
			return;
		}
	}

	// interact with world?
}
