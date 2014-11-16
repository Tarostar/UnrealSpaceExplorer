// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CustomPawn.h"
#include "SpaceExplorerPawn.generated.h"

/* Player */
UCLASS(config = Game)
class ASpaceExplorerPawn : public ACustomPawn
{
public:
	GENERATED_UCLASS_BODY()
		
	/** PUBLIC VARIABLES */
		
	/* StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
	TSubobjectPtr<class UStaticMeshComponent> Mesh;

private:
	/** PRIVATE FUNCTIONS */

	/* Begin AActor overrides */
	virtual void OnConstruction(const FTransform& Transform) override;
	/* End AActor overrides */

	/* Begin ACustomPawn overrides */
	virtual void SetPawnVisibility(bool bVisible) override;
	virtual void AttachCamera(TSubobjectPtr<class UCameraComponent> Camera) override;
	virtual void AttachSpringArm(TSubobjectPtr<class USpringArmComponent> SpringArm) override;
	/* End ACustomPawn overrides */
};
