

#pragma once

#include "GameFramework/PlayerController.h"
#include "CustomController.generated.h"

/**
 * Custom controller for custom player handling
 * Initial save system created from Rama's "Custom Save System To Binary Files" tutorial. Thanks!!!
 * This alpha version is just for testing, and simply saves / loads an integer for proof of concept.
 */
UCLASS()
class SPACEEXPLORER_API ACustomController : public APlayerController
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameController)
	int32 m_nTest;

public:

	bool SaveGameDataToFile(const FString& FullFilePathy);


	bool LoadGameDataFromFileCompressed(const FString& FullFilePath, int32& SaveDataInt32);
	/*bool SaveGameDataToFileCompressed(const FString& FullFilePath, int32& SaveDataInt32, FVector& SaveDataVector, TArray<FRotator>& SaveDataRotatorArray);*/

private:
	// FArchive is shared base class for FBufferArchive and FMemoryReader
	void SaveLoadData(FArchive& Ar, int32& SaveDataInt32);
};


