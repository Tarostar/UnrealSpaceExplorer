

#pragma once

#include "GameFramework/Actor.h"
#include "UsableObject.generated.h"

/**
 * Base item class for all objects in the world and inventory
 * Considerations and known issues:
 * - When item moves out of inventory - who becomes owner?
 * - Is it acceptable to turn on/off mesh rather than destroy/spawn item in/out of inventory
 * - Should everything be replicated (i.e. also textures, mesh...)?
 * - Difference between translate and location
 * - Does setting FTransform on AActor also set the component location?
 */
UCLASS()
class SPACEEXPLORER_API AUsableObject : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Item)
	FName m_name;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 m_nInvWidth;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 m_nInvHeight;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Item)
	bool m_bIsStackable;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 m_nCount;

	// texture in inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	UTexture2D * m_inventoryTexture;

	// DEFUNCT: mesh when in the world
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	TSubobjectPtr<class UStaticMeshComponent> m_mesh;*/

	UFUNCTION(BlueprintCallable, Category = Item)
	void InitMesh();

	UFUNCTION(BlueprintCallable, Category = Item)
	void Collect(AActor * pNewOwner);

	UFUNCTION(BlueprintCallable, Category = Item)
	void Drop();

	UFUNCTION(BlueprintCallable, Category = Item)
	bool IsVisibleInWorld();

	UFUNCTION(BlueprintCallable, Category = Item)
	bool InvokeAction();

	UFUNCTION(BlueprintCallable, Category = Item)
	int32 GetItemWidth();

	UFUNCTION(BlueprintCallable, Category = Item)
	int32 GetItemHeight();

	// TODO: these are for highlighting collectable items...

	UFUNCTION(BlueprintImplementableEvent)
	bool OnUsed(APawn* character);

	UFUNCTION(BlueprintImplementableEvent)
	bool StartFocusItem();

	UFUNCTION(BlueprintImplementableEvent)
	bool EndFocusItem();

	void SaveLoad(FArchive& Ar);

	/* get unique ID for item */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetID();

private:
	// TODO: this is just temporary until I figure out how to set a world owner when item dropped
	AActor* m_worldOwner;

	int32 ID;

	int32 Type;
};
