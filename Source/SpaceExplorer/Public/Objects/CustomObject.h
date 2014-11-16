// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CustomObject.generated.h"

// TODO: meshes for physical representation of objects set in specific child objects, and probably in blueprints only?

/**
 * The base template from which all objects in the world are created
 */
UCLASS()
class SPACEEXPLORER_API ACustomObject : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	// descriptive name
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	FName Name;

	// major object type
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	FName Type;

	// notify factions when in player's possession and they can notice
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	bool bNotifyFactions;

	// each round in a FactoryMachine cost equal to ProductionRate is covered
	// and when that matches or exceeds ProductionCost item is ready to be deployed.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	int32 ProductionCost;

	void SaveLoad(FArchive& Ar);
};
