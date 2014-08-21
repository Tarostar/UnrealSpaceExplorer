// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
#include "SpaceExplorerGameMode.h"
#include "SpaceExplorerPawn.h"
#include "SpaceHUD.h"

ASpaceExplorerGameMode::ASpaceExplorerGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// static character pawn
	//DefaultPawnClass = ASpaceExplorerPawn::StaticClass();

	// Character Pawn
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_SpaceExplorer.BP_SpaceExplorer'"));
	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	// HUD
	static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("Blueprint'/Game/HUD/BP_CustomHUD.BP_CustomHUD'"));
	//static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("Blueprint'/Game/ExampleContent/HUD/Blueprints/BP_HUD_Example.BP_HUD_Example'"));
	if (TheHUDOb.Object != NULL)
	{
		HUDClass = (UClass*)TheHUDOb.Object->GeneratedClass;
	}
}

void ASpaceExplorerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameMode::InitGame(MapName, Options, ErrorMessage);

	if (GEngine && GEngine->GameViewport)
	{
		// remove pause text
		GEngine->GameViewport->SetSuppressTransitionMessage(true);
	}
}

