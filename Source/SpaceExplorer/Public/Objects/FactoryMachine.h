// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomObject.h"
#include "GameFramework/Actor.h"
#include "FactoryMachine.generated.h"

// TODO: can this all be handled on server, or does it need replicating to clients?

USTRUCT()
struct FProductPriority
{
	GENERATED_USTRUCT_BODY()

	int32 Priority;
	TArray<TSubclassOf<class ACustomObject> > AvailableTypes;

	// Generate random number from 0 to priority total of all ProductionPlan items
	// This priorities [a] 10, [b] 1000, [c] 50 generates a random number from 0-1060
	// result: 0-9 a, 10-1009 b and 1010-1059 c.

	FProductPriority()
	{
		Priority = 1;
	}
};


/**
 * A Machine that produces objects for a factory to deploy
 */
UCLASS()
class SPACEEXPLORER_API AFactoryMachine : public AActor
{
	GENERATED_UCLASS_BODY()

protected:		

	// production cost produced each tick
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 ProductionRate;

	// how much has been produced so far (ProductionRate adds to this each tick)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 CurrentProduction;

	// minimum production before deploying objects
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	int32 MinimumProduction;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	TArray<ACustomObject*> ProductionQueue;

	// plan for what to add to the ProductionQueue
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Factory)
	TArray<FProductPriority> ProductionPlan;
};
