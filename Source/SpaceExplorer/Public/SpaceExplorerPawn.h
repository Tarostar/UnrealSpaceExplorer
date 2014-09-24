// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

class AUsableObject;
#include "InventoryObject.h"
#include "SpaceExplorerPawn.generated.h"

/* Player */
UCLASS(config = Game)
class ASpaceExplorerPawn : public APawn
{
public:
	GENERATED_UCLASS_BODY()
	
	/** PUBLIC FUNCTIONS */

	// INVENTORY OBJECTS and ITEMS

	/* Add an item to the inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItem(AUsableObject * pItem);

	/* Get an inventory based on its index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AInventoryObject* GetInventoryObjectFromIndex(int32 index);

	/* Get an inventory based on its ID */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	AInventoryObject* GetInventoryObjectFromID(int32 nID);

	/* Get number of inventories owned by this player */
	int32 GetInventoryObjectCount();

	/* invoke an action (item optional) */
	void InvokeAction(class DragObject& item);

	/* Runs on Server. Perform "OnUsed" on currently viewed UsableObject if implemented. */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = Inventory)
	virtual void UseItem(int32 nInventoryID, int32 nSlotIndex);

	/** PUBLIC VARIABLES */

	/* Max distance to use/focus on actors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float m_fMaxUseDistance;

	/* True only in first frame when focused on new usable actor. */
	bool m_bHasNewFocus;

	/* StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly)
	TSubobjectPtr<class UStaticMeshComponent> m_planeMesh;

	/* Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	TSubobjectPtr<class USpringArmComponent> m_springArm;

	/* Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly)
	TSubobjectPtr<class UCameraComponent> m_camera;

	// MOVEMENT

	/* How quickly forward speed changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fAcceleration;

	/* How quickly pawn can steer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fTurnSpeed;

	/* Max forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fMaxSpeed;

	/* Min forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fMinSpeed;

	/* Current forward speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fCurrentForwardSpeed;

	/* Current yaw speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fCurrentYawSpeed;

	/* Current pitch speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fCurrentPitchSpeed;

	/* Current roll speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float m_fCurrentRollSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CurrentZoom;

	/* Free look or follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool m_bFreeMouseLook;

	/* Minimum target distance of camera from player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float m_fMinimumArmTargetDistance;

	/* Third or first person view */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool m_bFirstPersonView;

	// DAMAGE

	/* Current damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float m_fCurrentDamage;

	/* Damage before destruction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float m_fMaxDamage;

private:
	/** PRIVATE FUNCTIONS */

	/* Begin APawn overrides */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	virtual void BeginPlay() override;
	/* End APawn overrides */

	/* Begin AActor overrides */
	virtual void Tick(float DeltaSeconds) override;
	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	/* End AActor overrides */

	/* Get actor derived from UsableObject currently looked at by the player */
	// class AUsableObject* GetUsableInView();

	// void HandleUsableObject();

	/* Bound to the vertical axis */
	void ThrustInput(float Val);

	/* Bound to the horizontal axis */
	void MoveUpInput(float Val);

	/* Turn left and right (left is negative value) */
	void MoveRightInput(float Val);

	/* Zoom in and out */
	void Zoom(float Val);

	/* Action Inputs */
	void CamReset();
	void StartMouseLook();
	void StopMouseLook();
	void ToggleFreeMouseLook();
	void ZoomIn();
	void ZoomOut();
	void ToggleFirstPerson();
	void Fire();
	void LMBPressed();
	void LMBRelease();

	/* check with HUD if interacting with an item - otherwise interact with world */
	void Interact();

	/* Enter mouse look state */
	void SetMouseLook(bool bMouseLook);

	// INVENTORY OBJECTS

	/* Helper to find and assign first unique inventory ID */
	int32 AssignUniqueInventoryID();

	/* Helper to get the index of a player inventory from the unique inventory ID */
	int32 GetInventoryIndexFromID(int32 nID);

	/* open/close all inventory containers */
	void ToggleAllInventory();

	/* open / close individual inventory objects */
	void ToggleInventoryOne();
	void ToggleInventoryTwo();
	void ToggleInventoryThree();
	bool ToggleInventory(int32 nIndex);

	/* Delete key input - removes any selected actions in Actionbar */
	void Delete();

	/** PRIVATE VARIABLES */

	/* Actor derived from UsableObject currently in center-view. */
	// AUsableObject* FocusedUsableObject;

	/* array of all inventory containers */
	UPROPERTY(Replicated)
	TArray<AInventoryObject*> m_inventoryObjects;
};
