// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "SpaceExplorerGameMode.generated.h"

UCLASS(minimalapi)
class ASpaceExplorerGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
};



