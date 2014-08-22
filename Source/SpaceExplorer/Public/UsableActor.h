

#pragma once

#include "Engine/StaticMeshActor.h"
#include "UsableActor.generated.h"

/**
 * Base class for all actors that can be used or activated
 */
UCLASS()
class SPACEEXPLORER_API AUsableActor : public AStaticMeshActor
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintImplementableEvent)
	bool OnUsed(APawn* character);

	UFUNCTION(BlueprintImplementableEvent)
	bool StartFocusItem();

	UFUNCTION(BlueprintImplementableEvent)
	bool EndFocusItem();
	
};
