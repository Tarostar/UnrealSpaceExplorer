// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryObject.h"
#include "GameFramework/Actor.h"
#include "FactoryPortal.generated.h"

/**
 * One of potentially many deploy locations or areas for a factory where objects are placed into the world (or inventory)
 */
UCLASS()
class SPACEEXPLORER_API AFactoryPortal : public AActor
{
	GENERATED_UCLASS_BODY()

protected:	
	// only active portals can spawn objects
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	bool bActive;

	// location of this portal
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	FVector Position;

	// objects can spawn in any direction from Position within this DeployRadius (use 0 for exact location deployment)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 DeployRadius;

	// higher priority increases chance factory will use this factory when randomly deploying
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 Priority;

	// when set, points to an inventory where items should be deployed if within the DeployRadius
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	AInventoryObject* DeployInventory;

	// if set, when no DeployInventory is within DeployRadius the portal will refuse to deploy (potentially blocking production), 
	// otherwise will simply deploy randomly within DeployRadius
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	bool bOnlyDeployToInventory;
};
