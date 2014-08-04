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
	SpringArm = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;

	DefaultSpringRotation = SpringArm->GetComponentRotation();

	// Create camera component 
	Camera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUseControllerViewRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 50.f;
	MaxSpeed = 4000.f;
	MinSpeed = 500.f;
	CurrentForwardSpeed = 500.f;
}

void ASpaceExplorerPawn::OnConstruction(const FTransform& Transform)
{
	//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Yellow, TEXT("Default Spring Rot: ") + DefaultSpringRotation.ToString());
	
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

	// rotate arm if necessary
	if (!SpringArm->bUseControllerViewRotation && !DefaultSpringRotation.Equals(CurrentSpringRotation, 1.0f))
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Default: ") + DefaultSpringRotation.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Cur: ") + CurrentSpringRotation.ToString());
		}*/

		// Smoothly interpolate
		CurrentSpringRotation = FMath::RInterpTo(CurrentSpringRotation, DefaultSpringRotation, GetWorld()->GetDeltaSeconds(), 2.f);
		SpringArm->SetWorldRotation(CurrentSpringRotation);
		ClientSetRotation(CurrentSpringRotation);
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

	// Fine up and right axes
	InputComponent->BindAxis("Thrust", this, &ASpaceExplorerPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &ASpaceExplorerPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &ASpaceExplorerPawn::MoveRightInput);

	InputComponent->BindAction("ResetCamera", IE_Pressed, this, &ASpaceExplorerPawn::CamReset);
	InputComponent->BindAction("MouseLook", IE_Pressed, this, &ASpaceExplorerPawn::StartMouseLook);
	InputComponent->BindAction("MouseLook", IE_Released, this, &ASpaceExplorerPawn::StopMouseLook);

	InputComponent->BindAxis("Turn", this, &ASpaceExplorerPawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ASpaceExplorerPawn::AddControllerPitchInput);
}

void ASpaceExplorerPawn::CamReset()
{
	CurrentSpringRotation = SpringArm->GetComponentRotation();
}

void ASpaceExplorerPawn::StartMouseLook()
{
	// rotate the arm based on the controller
	SpringArm->bUseControllerViewRotation = true;
}

void ASpaceExplorerPawn::StopMouseLook()
{
	// set arm so cam is back behind player
	SpringArm->bUseControllerViewRotation = false;
	CamReset();
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
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
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