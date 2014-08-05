// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
#include "SpaceExplorerGameMode.h"
#include "SpaceExplorerPawn.h"
#include "SpaceHUD.h"

ASpaceExplorerGameMode::ASpaceExplorerGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = ASpaceExplorerPawn::StaticClass();

	// code for if I want to use blueprint
	/*static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_UFO.BP_UFO'"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}*/

	// HUD
	static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("Blueprint'/Game/Blueprints/BP_SpaceHUD.BP_SpaceHUD'"));
	if (TheHUDOb.Object != NULL)
	{
		HUDClass = (UClass*)TheHUDOb.Object->GeneratedClass;
	}

	//HUDClass = ASpaceHUD::StaticClass();
}
