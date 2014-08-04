// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
#include "SpaceExplorerGameMode.h"
#include "SpaceExplorerPawn.h"

ASpaceExplorerGameMode::ASpaceExplorerGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our flying pawn
	//DefaultPawnClass = ASpaceExplorerPawn::StaticClass();

	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_UFO.BP_UFO'"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	// static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnClassFinder(TEXT("Blueprint'/Game/Blueprints/BP_UFO.BP_UFO'"));
	// DefaultPawnClass = PlayerPawnClassFinder.Class;
}
