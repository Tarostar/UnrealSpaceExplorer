// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ObjectTemplate.generated.h"

// TODO: meshes for physical representation of objects set in specific child objects, and probably in blueprints only?

/**
 * The base template from which all objects in the world are created
 */
UCLASS()
class SPACEEXPLORER_API AObjectTemplate : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	// notify factions when in player's possession and they can notice
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	bool bNotifyFactions;

	// each round in a FactoryMachine cost equal to ProductionRate is covered
	// and when that matches or exceeds ProductionCost item is ready to be deployed.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	int32 ProductionCost;

	// major object type
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	FName Type;

	void SaveLoad(FArchive& Ar);
};
