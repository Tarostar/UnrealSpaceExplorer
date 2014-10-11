/*
Custom Level BP Template
*/

#include "SpaceExplorer.h"
#include "CustomLevelScriptActor.h"

ACustomLevelScriptActor::ACustomLevelScriptActor(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	CustomLevelName = NAME_None;
}

//Actor.h 		ReceiveBeginPlay()
void ACustomLevelScriptActor::ReceiveBeginPlay()
{
	Super::ReceiveBeginPlay();

	//UE_LOG(YourLog,Error,TEXT("STREAMING LEVEL BEGIN PLAY %s"), *GetName());
}

void ACustomLevelScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/*
	UE_LOG(YourLog,Error,TEXT("STREAMING LEVEL UNSTREAMED (name should not be none) %s"), *GetName());
	UE_LOG(YourLog,Error,TEXT("Victory!!!"));
	UE_LOG(YourLog,Error,TEXT("RUN ALL UNSTREAMING LEVEL LOGIC HERE"));
	*/

	// Save level just before exiting
	CustomLevel__UpdateLevelSaveData();
}

void ACustomLevelScriptActor::CustomLevel__UpdateLevelSaveData()
{
	//Save Level Data
}

void ACustomLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//UE_LOG(YourLog,Warning,TEXT("Level Name Is: %s"), *SolusLevelName);
}