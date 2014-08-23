

#pragma once

#include "GameFramework/Actor.h"
#include "InventoryObject.generated.h"

/**
 * Base class for inventory objects such as bags, sacks, containers and anything that can hold items
 */
UCLASS()
class SPACEEXPLORER_API AInventoryObject : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 InvWidthCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	int32 InvHeightCount;

	
};
