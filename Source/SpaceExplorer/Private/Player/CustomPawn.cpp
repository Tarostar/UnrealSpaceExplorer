// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "CustomPawn.h"
#include "Net/UnrealNetwork.h"
#include "UsableObject.h"
#include "CustomHUD.h"
#include "InventoryObject.h"
#include "CustomController.h"

ACustomPawn::ACustomPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// Create a spring arm component
	m_fMinimumArmTargetDistance = 160.0f;
	m_springArm = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("SpringArm0"));
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
	m_camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set default handling parameters
	m_fAcceleration = 1000.f;
	m_fTurnSpeed = 100.f;
	m_fMaxSpeed = 8000.f;
	m_fMinSpeed = 0.f;
	m_fCurrentForwardSpeed = 0.f;

	CurrentZoom = 1.f;
	m_bFreeMouseLook = false;

	// Default damage tolerance
	m_fCurrentDamage = 0.f;
	m_fMaxDamage = 8000.f;

	bool m_bFirstPersonView = false;
}

void ACustomPawn::BeginPlay()
{
	Super::BeginPlay();

	AttachSpringArm(m_springArm);

	AddInventory(10, 10);
	AddInventory(10, 5);
	AddInventory(5, 5);

}

void ACustomPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// axes
	InputComponent->BindAxis("Thrust", this, &ACustomPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &ACustomPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &ACustomPawn::MoveRightInput);
	InputComponent->BindAxis("Turn", this, &ACustomPawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ACustomPawn::AddControllerPitchInput);
	InputComponent->BindAxis("Zoom", this, &ACustomPawn::Zoom);

	// actions
	InputComponent->BindAction("ResetCamera", IE_Pressed, this, &ACustomPawn::CamReset);
	InputComponent->BindAction("MouseLook", IE_Pressed, this, &ACustomPawn::StartMouseLook);
	InputComponent->BindAction("MouseLook", IE_Released, this, &ACustomPawn::StopMouseLook);
	InputComponent->BindAction("CameraFollow", IE_Pressed, this, &ACustomPawn::ToggleFreeMouseLook);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ACustomPawn::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ACustomPawn::ZoomOut);
	InputComponent->BindAction("ToggleFirstPerson", IE_Pressed, this, &ACustomPawn::ToggleFirstPerson);

	InputComponent->BindAction("ToggleAllInventory", IE_Pressed, this, &ACustomPawn::ToggleAllInventory);
	InputComponent->BindAction("ToggleInventoryOne", IE_Pressed, this, &ACustomPawn::ToggleInventoryOne);
	InputComponent->BindAction("ToggleInventoryTwo", IE_Pressed, this, &ACustomPawn::ToggleInventoryTwo);
	InputComponent->BindAction("ToggleInventoryThree", IE_Pressed, this, &ACustomPawn::ToggleInventoryThree);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ACustomPawn::Interact);
	InputComponent->BindAction("Delete", IE_Pressed, this, &ACustomPawn::Delete);

	// TODO: this should be interact, pick up or similar instead
	InputComponent->BindAction("LMButton", IE_Pressed, this, &ACustomPawn::LMBPressed);
	InputComponent->BindAction("LMButton", IE_Released, this, &ACustomPawn::LMBRelease);

	InputComponent->BindAction("QuickSave", IE_Pressed, this, &ACustomPawn::QuickSave);
	InputComponent->BindAction("QuickLoad", IE_Pressed, this, &ACustomPawn::QuickLoad);
}

void ACustomPawn::Tick(float DeltaSeconds)
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

	if (!m_bFreeMouseLook && !m_bFirstPersonView && !m_springArm->bUsePawnControlRotation)
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
	else if (!m_bFreeMouseLook && m_bFirstPersonView && !m_camera->bUsePawnControlRotation)
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
}

void ACustomPawn::ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	m_fCurrentForwardSpeed = 0.f;
}

void ACustomPawn::AddControllerYawInput(float Val)
{
	if (m_springArm->bUsePawnControlRotation || m_camera->bUsePawnControlRotation)
		Super::AddControllerYawInput(Val);
}

void ACustomPawn::AddControllerPitchInput(float Val)
{
	if (m_springArm->bUsePawnControlRotation || m_camera->bUsePawnControlRotation)
		Super::AddControllerPitchInput(Val);
}

void ACustomPawn::ThrustInput(float Val)
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

void ACustomPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float m_fTargetPitchSpeed = (Val * m_fTurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	m_fTargetPitchSpeed += (FMath::Abs(m_fCurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	m_fCurrentPitchSpeed = FMath::FInterpTo(m_fCurrentPitchSpeed, m_fTargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void ACustomPawn::MoveRightInput(float Val)
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

void ACustomPawn::Interact()
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

void ACustomPawn::LMBPressed()
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

void ACustomPawn::LMBRelease()
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

void ACustomPawn::CamReset()
{
	// set controller rotation (for when resuming camera look control)

	// m_springArm->SetWorldRotation(GetActorRotation());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Rot: ") + TargetSpringRotation.ToString());
}

void ACustomPawn::ZoomIn()
{
	Zoom(-1.0f);
}

void ACustomPawn::ZoomOut()
{
	Zoom(1.0f);
}

void ACustomPawn::Fire()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Bang!"));
}

void ACustomPawn::ToggleFreeMouseLook()
{
	m_bFreeMouseLook = !m_bFreeMouseLook;

	SetMouseLook(m_bFreeMouseLook);
}

void ACustomPawn::StartMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (m_bFreeMouseLook)
		return;

	SetMouseLook(true);
}

void ACustomPawn::SetMouseLook(bool bMouseLook)
{
	// rotate the camera or arm based on the controller
	if (m_bFirstPersonView)
	{
		// set camera to current arm position
		ClientSetRotation(m_camera->GetComponentRotation());
		m_camera->bUsePawnControlRotation = bMouseLook;
	}
	else
	{
		// set controller to current arm position
		ClientSetRotation(m_springArm->GetComponentRotation());
		m_springArm->bUsePawnControlRotation = bMouseLook;
	}
}

void ACustomPawn::StopMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (m_bFreeMouseLook)
		return;

	SetMouseLook(false);
}


void ACustomPawn::ToggleFirstPerson()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("ToggleFirstPerson"));

	if (m_bFirstPersonView)
	{
		// toggle back to springarm
		m_camera->AttachTo(m_springArm, USpringArmComponent::SocketName);
		m_camera->RelativeLocation = FVector(0, 0, 0);
		SetPawnVisibility(true);
	}
	else
	{
		// toggle to first person (better to use socket for cockpit and do relative to that or at least get the location at the tip of the ship...)
		AttachCamera(m_camera);
		m_camera->RelativeLocation = FVector(100.0f, 0, 00.0f);

		SetPawnVisibility(false);
	}

	m_bFirstPersonView = !m_bFirstPersonView;

	SetMouseLook(m_bFirstPersonView);
}

void ACustomPawn::Zoom(float Val)
{
	if (FMath::IsNearlyEqual(Val, 0.f))
		return; // nothing going on...

	if (Val < 0.f && CurrentZoom <= 1.0f)
		return; // can't zoom in further

	if (Val > 0.f && CurrentZoom >= 3.0f)
		return; // can't zoom out further

	CurrentZoom += Val;
}

// TODO: also move inputs and then maybe create interface that all children classes inherit...

void ACustomPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ACustomPawn, m_inventoryObjects);
}

int32 ACustomPawn::AddInventory(int32 Width, int32 Height)
{
	UWorld* const World = GetWorld();
	if (World)
	{
		AInventoryObject * invObject = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());
		if (invObject)
		{
			invObject->Init(AssignUniqueInventoryID(), Width, Height);
			return m_inventoryObjects.Add(invObject);
		}
	}

	return -1;
}

void ACustomPawn::ToggleAllInventory()
{
	if (!Controller)
	{
		return;
	}

	ACustomController* pc = Cast<ACustomController>(Controller);
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

bool ACustomPawn::ToggleInventory(int32 nIndex)
{
	if (!Controller || !m_inventoryObjects.IsValidIndex(nIndex))
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryOne Failed because missing controller or index is invalid"));
		return false;
	}

	ACustomController* pc = Cast<ACustomController>(Controller);
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

void ACustomPawn::ToggleInventoryOne()
{
	ToggleInventory(0);
}

void ACustomPawn::ToggleInventoryTwo()
{
	ToggleInventory(1);
}

void ACustomPawn::ToggleInventoryThree()
{
	ToggleInventory(2);
}

bool ACustomPawn::AddItem(AUsableObject * pItem)
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

int32 ACustomPawn::AssignUniqueInventoryID()
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

int32 ACustomPawn::GetInventoryIndexFromID(int32 nID)
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

AInventoryObject* ACustomPawn::GetInventoryObjectFromIndex(int32 nIndex)
{
	if (m_inventoryObjects.IsValidIndex(nIndex))
	{
		return m_inventoryObjects[nIndex];
	}

	return nullptr;
}

AInventoryObject* ACustomPawn::GetInventoryObjectFromID(int32 nID)
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

int32 ACustomPawn::GetInventoryObjectCount()
{
	return m_inventoryObjects.Num();
}

void ACustomPawn::Delete()
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

void ACustomPawn::InvokeAction(class DragObject& item)
{
	switch (item.GetType())
	{
	case EActionType::Use: UseItem(item.GetInventoryID(), item.GetSlotIndex());
		return;
	case EActionType::Attack: Fire();
		return;
	}
}

void ACustomPawn::UseItem_Implementation(int32 nInventoryID, int32 nSlotIndex)
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

bool ACustomPawn::UseItem_Validate(int32 nInventoryID, int32 nSlotIndex)
{
	// No special server-side validation performed.
	return true;
}

void ACustomPawn::QuickSave()
{
	if (!Controller)
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryOne Failed because missing controller or index is invalid"));
		return;
	}

	ACustomController* pc = Cast<ACustomController>(Controller);
	if (pc)
	{
		pc->SaveGameDataToFile("G:\\UESaveTest\\quicksave.sav", this);
	}
}

void ACustomPawn::QuickLoad()
{
	if (!Controller)
	{
		//UE_LOG(Pawn, Error, TEXT("ToggleInventoryOne Failed because missing controller or index is invalid"));
		return;
	}

	ACustomController* pc = Cast<ACustomController>(Controller);
	if (pc == nullptr)
	{
		return;
	}

	pc->LoadGameDataFromFileCompressed("G:\\UESaveTest\\quicksave.sav", this);
}

void ACustomPawn::SaveLoad(FArchive& Ar)
{
	// Saving / Loading member variables
	Ar << m_bFreeMouseLook;
	Ar << m_bFirstPersonView;

	// do we need to save spring arm and camera?

	// MOVEMENT

	Ar << m_fAcceleration;
	Ar << m_fTurnSpeed;
	Ar << m_fMaxSpeed;
	Ar << m_fMinSpeed;
	Ar << m_fCurrentForwardSpeed;
	Ar << m_fCurrentYawSpeed;
	Ar << m_fCurrentPitchSpeed;
	Ar << m_fCurrentRollSpeed;
	Ar << CurrentZoom;
	Ar << m_fMinimumArmTargetDistance;
	Ar << m_fCurrentDamage;
	Ar << m_fMaxDamage;

	// Saving/Loading player location and rotation

	FVector PlayerLocation;
	FRotator PlayerRotation;
	if (!Ar.IsLoading())
	{
		PlayerLocation = GetActorLocation();
		PlayerRotation = GetActorRotation();
	}
	Ar << PlayerLocation;
	Ar << PlayerRotation;
	if (Ar.IsLoading())
	{
		SetActorLocation(PlayerLocation);
		SetActorRotation(PlayerRotation);
	}

	// Saving/Loading inventory objects

	int32 InventoryCount = m_inventoryObjects.Num();
	Ar << InventoryCount;

	if (Ar.IsLoading())
	{
		m_inventoryObjects.Empty();

		UWorld* const World = GetWorld();
		if (World)
		{
			for (int i = 0; i < InventoryCount; i++)
			{
				AInventoryObject * InvObject = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());
				if (InvObject)
				{
					InvObject->SaveLoad(Ar);
					m_inventoryObjects.Add(InvObject);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < InventoryCount; i++)
		{
			m_inventoryObjects[i]->SaveLoad(Ar);
		}
	}
}

void ACustomPawn::SetPawnVisibility(bool bVisible)
{
	// do nothing
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("SetPawnVisibility not overriden"));
}

void ACustomPawn::AttachCamera(TSubobjectPtr<class UCameraComponent> Camera)
{
	// do nothing
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("AttachCamera not overriden"));
}

void ACustomPawn::AttachSpringArm(TSubobjectPtr<class USpringArmComponent> SpringArm)
{
	// do nothing
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("AttachSpringArm not overriden"));
}
