
/*
* Custom Level Script Actor Template
* Based on Template By Rama that he used for the really cool Solus project.
* Thank You!
*/

#pragma once

#include "Engine/LevelScriptActor.h"

//#include "EngineLevelScriptClasses.h"

#include "CustomLevelScriptActor.generated.h"

UCLASS()
class ACustomLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

	/** A Custom name for each Level, specified in Level BP Default Properties! */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = CustomLevel)
	FName CustomLevelName;

	//===========
	// BP Nodes
	//===========
public:
	/** This triggers a save / update of all the Save Game level data. This should be called before the level is unloaded/unstreamed via blueprints.*/
	UFUNCTION(BlueprintCallable, Category = CustomLevel)
		void CustomLevel__UpdateLevelSaveData();


	
protected:
	/* Overrides */
	virtual void ReceiveBeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* Tick */
	virtual void Tick(float DeltaSeconds) override;

};