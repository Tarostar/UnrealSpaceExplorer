// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
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
}

void ASpaceExplorerPawn::OnConstruction(const FTransform& Transform)
{
	// Override handling parameters
	m_fAcceleration = 500.f;
	m_fTurnSpeed = 200.f;
	m_fMaxSpeed = 4000.f;

	// Override damage tolerance
	m_fMaxDamage = 4000.f;

}

void ASpaceExplorerPawn::SetPawnVisibility(bool bVisible)
{
	// cheating...
	m_planeMesh->SetVisibility(bVisible);

	// TODO: this does not work
	//m_planeMesh->SetHiddenInGame
}

void ASpaceExplorerPawn::AttachCamera(TSubobjectPtr<class UCameraComponent> Camera)
{
	if (Camera)
	{
		Camera->AttachTo(m_planeMesh);
	}
}

void ASpaceExplorerPawn::AttachSpringArm(TSubobjectPtr<class USpringArmComponent> SpringArm)
{
	if (SpringArm)
	{
		SpringArm->AttachTo(m_planeMesh);
	}
}
