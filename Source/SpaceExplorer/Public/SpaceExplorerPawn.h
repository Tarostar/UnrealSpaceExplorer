// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

class AUsableObject;
#include "InventoryObject.h"
#include "SpaceExplorerPawn.generated.h"

UCLASS(config = Game)
class ASpaceExplorerPawn : public APawn
{
public:
	GENERATED_UCLASS_BODY()

	/** Use the actor currently in view (if derived from UsableObject) */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = PlayerAbility)
	virtual void Use();

public:
	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
		TSubobjectPtr<class UStaticMeshComponent> PlaneMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
		TSubobjectPtr<class USpringArmComponent> SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
		TSubobjectPtr<class UCameraComponent> Camera;

	// MOVEMENT

	/** How quickly forward speed changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float MinSpeed;

	/** Current forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float CurrentForwardSpeed;

	/** Current yaw speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float CurrentYawSpeed;

	/** Current pitch speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float CurrentPitchSpeed;

	/** Current roll speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float CurrentRollSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float CurrentZoom;

	// free look or follow pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool bFreeMouseLook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float MinimumArmTargetDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		bool bFirstPersonView;

	// DAMAGE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float CurrentDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
		float MaxDamage;

	/* Max distance to use/focus on actors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float MaxUseDistance;

private:
	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	virtual void BeginPlay() override;
	// End APawn overrides

	/** Get actor derived from UsableObject currently looked at by the player */
	class AUsableObject* GetUsableInView();
	void HandleUsableObject();

	/** Bound to the vertical axis */
	void ThrustInput(float Val);

	/** Bound to the horizontal axis */
	void MoveUpInput(float Val);

	/** */
	void MoveRightInput(float Val);

	/** Axis controls */
	void Zoom(float Val);
	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	// End AActor overrides

	/** Action Input */
	void CamReset();
	void StartMouseLook();
	void StopMouseLook();
	void Fire();
	void ToggleFreeMouseLook();
	void ZoomIn();
	void ZoomOut();
	void ToggleFirstPerson();

	/** Helpers */
	void SetMouseLook(bool bMouseLook);

private:
	/* True only in first frame when focused on new usable actor. */
	bool bHasNewFocus;

	/* Actor derived from UsableObject currently in center-view. */
	AUsableObject* FocusedUsableObject;

	/* open/close all inventory containers */
	void ToggleAllInventory();
	/* open / close first inventory object */
	void ToggleInventory1();

	/* array of all inventory containers */
	UPROPERTY()
	TArray<AInventoryObject*> m_inventoryObjects;
};
