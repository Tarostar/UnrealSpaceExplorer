

#pragma once

#include "CustomObject.h"
#include "Item.generated.h"

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
class SPACEEXPLORER_API AItem : public ACustomObject
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	int32 m_nInvWidth;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	int32 m_nInvHeight;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	bool m_bIsStackable;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Object)
	int32 m_nCount;

	// texture in inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Object)
	UTexture2D * m_inventoryTexture;

	// DEFUNCT: mesh when in the world
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Object)
	TSubobjectPtr<class UStaticMeshComponent> m_mesh;
	
	UFUNCTION(BlueprintCallable, Category = Object)
	void InitMesh();

	UFUNCTION(BlueprintCallable, Category = Object)
	void Collect(AActor * pNewOwner);

	UFUNCTION(BlueprintCallable, Category = Object)
	void Drop();

	UFUNCTION(BlueprintCallable, Category = Object)
	bool IsVisibleInWorld();

	UFUNCTION(BlueprintCallable, Category = Object)
	bool InvokeAction();

	UFUNCTION(BlueprintCallable, Category = Object)
	int32 GetItemWidth();

	UFUNCTION(BlueprintCallable, Category = Object)
	int32 GetItemHeight();
	
	void SaveLoad(FArchive& Ar);

	UFUNCTION(BlueprintImplementableEvent)
	bool OnUsed(APawn* character);
	
private:
	// TODO: this is just temporary until I figure out how to set a world owner when item dropped
	AActor* m_worldOwner;

};
