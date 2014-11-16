// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FactoryMachine.h"
#include "FactoryPortal.h"
#include "CustomObject.h"
#include "GameFramework/Actor.h"
#include "ObjectFactory.generated.h"

// TODO: review replication - especially on TArrays

// TODO: meshes for physical representation of factories set in blueprints only?

/**
 * Factories are responsible for all in-game created objects. Even player crafting is a Factory.
 */
UCLASS()
class SPACEEXPLORER_API AObjectFactory : public AActor
{
	GENERATED_UCLASS_BODY()

protected:
	// only active factories produce objects
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	bool bActive;

	// all the portals where objects can be deployed by this factory
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	TArray<AFactoryPortal*> DeployPortals;

	// the machines that produce objects that can then be deployed
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	TArray<AFactoryMachine*> Machines;

	// TODO: design factions...
	// factions who "own" this factory (and in turn may claim ownership of all objects deployed by this factory)
	// UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
//	TArray<AFaction> FactionOwners;

	// notify factions about factory (deployed objects typically inherit this setting, along with factory owners)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	bool bNotifyFactions;
		
	// how much damage it can take (only factories with a physical presence can take damage) - destroyed at 0
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 FactoryIntegrity;

	/** Called from main actor tick function to implement custom code at the appropriate point in the tick */
	virtual void Tick(float DeltaSeconds) override;

	void PopulateQueue();
	
	void DeployObject();
	
};

