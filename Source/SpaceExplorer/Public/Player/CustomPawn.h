// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class AItem;
#include "InventoryObject.h"
#include "GameFramework/Pawn.h"
#include "CustomPawn.generated.h"

/**
 * 
 */
UCLASS()
class SPACEEXPLORER_API ACustomPawn : public APawn
{
public:
	GENERATED_UCLASS_BODY()

	/** PUBLIC FUNCTIONS */

	void SaveLoad(FArchive& Ar);

	// INVENTORY OBJECTS and ITEMS

	/* Add new inventory object - returns index */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 AddInventory(int32 Width, int32 Height);

	/* Add an item to the inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddItem(AItem * pItem);

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

	/* Runs on Server. Perform "OnUsed" on currently viewed Item if implemented. */
	UFUNCTION(BlueprintCallable, WithValidation, Server, Reliable, Category = Inventory)
	virtual void UseItem(int32 nInventoryID, int32 nSlotIndex);

	/** PUBLIC VARIABLES */
	/* Free look or follow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool m_bFreeMouseLook;

	/* Third or first person view */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	bool m_bFirstPersonView;

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

	/* Minimum target distance of camera from player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float m_fMinimumArmTargetDistance;

	// DAMAGE

	/* Current damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float m_fCurrentDamage;

	/* Damage before destruction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float m_fMaxDamage;

private:
	/** PRIVATE FUNCTIONS */

	virtual void SetPawnVisibility(bool bVisible);
	virtual void AttachCamera(TSubobjectPtr<class UCameraComponent> Camera);
	virtual void AttachSpringArm(TSubobjectPtr<class USpringArmComponent> SpringArm);

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
	void QuickSave();
	void QuickLoad();

	/* Begin APawn overrides */
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	virtual void BeginPlay() override;
	/* End APawn overrides */

	/* Begin AActor overrides */
	virtual void Tick(float DeltaSeconds) override;
	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void AddControllerPitchInput(float Val) override;
	/* End AActor overrides */

	/* Bound to the vertical axis */
	void ThrustInput(float Val);

	/* Bound to the horizontal axis */
	void MoveUpInput(float Val);

	/* Turn left and right (left is negative value) */
	void MoveRightInput(float Val);

	/* Enter mouse look state */
	void SetMouseLook(bool bMouseLook);

	/* Zoom in and out */
	void Zoom(float Val);

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

	/* check with HUD if interacting with an item - otherwise interact with world */
	void Interact();

	/** PRIVATE VARIABLES */

	/* array of all inventory containers */
	UPROPERTY(Replicated)
	TArray<AInventoryObject*> m_inventoryObjects;



};
