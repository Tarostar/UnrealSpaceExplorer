

#pragma once

#include "GameFramework/Actor.h"
#include "UsableObject.generated.h"

/**
 * 
 */
UCLASS()
class SPACEEXPLORER_API AUsableObject : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 m_nInvWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 m_nInvHeight;

	UFUNCTION(BlueprintImplementableEvent)
	bool OnUsed(APawn* character);

	UFUNCTION(BlueprintImplementableEvent)
	bool StartFocusItem();

	UFUNCTION(BlueprintImplementableEvent)
	bool EndFocusItem();
	
};
