// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
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
	SpringArm->CameraLagSpeed = 0.f;
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
}

void ASpaceExplorerPawn::OnConstruction(const FTransform& Transform)
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, TEXT("Default Spring Rot: ") + TargetSpringRotation.ToString());
	
}

void ASpaceExplorerPawn::Tick(float DeltaSeconds)
{
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

	if (!bFreeMouseLook && !SpringArm->bUseControllerViewRotation)
	{
		// user is not rotating camera, smoothly interpolate to target position

		FRotator CurrentSpringRotation = SpringArm->GetComponentRotation();
		FRotator TargetRot = GetActorRotation();

		// set spring arm back
		if (!TargetRot.Equals(CurrentSpringRotation, 0.000001f))
		{
			CurrentSpringRotation = FMath::RInterpTo(CurrentSpringRotation, TargetRot, GetWorld()->GetDeltaSeconds(), 2.f);
			SpringArm->SetWorldRotation(CurrentSpringRotation);
		}
	}

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
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
	InputComponent->BindAction("Fire", IE_Pressed, this, &ASpaceExplorerPawn::Fire);
	InputComponent->BindAction("CameraFollow", IE_Pressed, this, &ASpaceExplorerPawn::FreeMouseLook);
	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ASpaceExplorerPawn::ZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ASpaceExplorerPawn::ZoomOut);
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

void ASpaceExplorerPawn::FreeMouseLook()
{
	bFreeMouseLook = !bFreeMouseLook;

	// enable or disable mouse control of camera depending on mode
	if (bFreeMouseLook)
		SpringArm->bUseControllerViewRotation = true;
	else
		SpringArm->bUseControllerViewRotation = false;
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

	// set controller to current arm position
	ClientSetRotation(SpringArm->GetComponentRotation());

	// rotate the arm based on the controller
	SpringArm->bUseControllerViewRotation = true;
}

void ASpaceExplorerPawn::StopMouseLook()
{
	// check mode - i.e. if camera follow is disable mouse look is default
	if (bFreeMouseLook)
		return;
	
	// set arm so cam is back behind player
	CamReset();
	SpringArm->bUseControllerViewRotation = false;
}

void ASpaceExplorerPawn::Zoom(float Val)
{
	if (FMath::IsNearlyEqual(Val, 0.f))
		return; // nothing going on...

	if(Val < 0.f && CurrentZoom <= 1.0f)
		return; // can't zoom in further

	if (Val > 0.f && CurrentZoom >= 3.0f)
		return; // can't zoom out further

	CurrentZoom += Val;

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("Zoom: ") + FString::SanitizeFloat(Val));
	
}

void ASpaceExplorerPawn::AddControllerYawInput(float Val)
{
	if (SpringArm->bUseControllerViewRotation)
		Super::AddControllerYawInput(Val);
}

void ASpaceExplorerPawn::AddControllerPitchInput(float Val)
{
	if (SpringArm->bUseControllerViewRotation)
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