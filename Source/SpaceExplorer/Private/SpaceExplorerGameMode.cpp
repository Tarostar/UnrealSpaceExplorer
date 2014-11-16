// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "SpaceExplorer.h"
#include "SpaceExplorerGameMode.h"
#include "SpaceExplorerPawn.h"
//#include "ItemFactory.h";
// #include "Item.h"
#include "SpaceHUD.h"

ASpaceExplorerGameMode::ASpaceExplorerGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	// static character pawn
	//DefaultPawnClass = ASpaceExplorerPawn::StaticClass();

	// Character Pawn
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_SpaceExplorer.BP_SpaceExplorer'"));
	if (PlayerPawnObject.Object != nullptr)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	// HUD
	static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("Blueprint'/Game/HUD/BP_CustomHUD.BP_CustomHUD'"));
	// static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("Blueprint'/Game/HUD/BP_MainHUD.BP_MainHUD'"));
	if (TheHUDOb.Object != nullptr)
	{
		HUDClass = (UClass*)TheHUDOb.Object->GeneratedClass;
	}

	// controller
	static ConstructorHelpers::FObjectFinder<UBlueprint> CustomControllerOb(TEXT("Blueprint'/Game/Blueprints/BP_CustomController.BP_CustomController'"));
	if (CustomControllerOb.Object != nullptr)
	{
		PlayerControllerClass = (UClass*)CustomControllerOb.Object->GeneratedClass;
	}

	// game state
	static ConstructorHelpers::FObjectFinder<UBlueprint> CustomGameStateOb(TEXT("Blueprint'/Game/Blueprints/BP_CustomGameState.BP_CustomGameState'"));
	if (CustomGameStateOb.Object != nullptr)
	{
		GameStateClass = (UClass*)CustomGameStateOb.Object->GeneratedClass;
	}

	/*Cast<AMyWorldSettings>(GetWorld()->GetWorldSettings())->GetMyVar();*/

	// m_nNextObjectID = 0;
}

void ASpaceExplorerGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameMode::InitGame(MapName, Options, ErrorMessage);

	if (GEngine && GEngine->GameViewport)
	{
		// remove pause text
		GEngine->GameViewport->SetSuppressTransitionMessage(true);
	}

	//m_itemFactory.Init();
}

// TODO: review
/*
AItem* ASpaceExplorerGameMode::CreateItem()
{
	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		AItem * object = World->SpawnActor<AItem>(AItem::StaticClass());

		if (object)
		{
			object->Init(AssignUniqueObjectID(), 5, 5);
			invObject->m_inventorySlots.SetNum(invObject->m_nInvHeightCount * invObject->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject);
		}

		AInventoryObject * invObject2 = World->SpawnActor<AInventoryObject>(AInventoryObject::StaticClass());

		if (invObject2)
		{
			invObject2->Init(AssignUniqueInventoryID(), 10, 2);
			invObject2->m_inventorySlots.SetNum(invObject2->m_nInvHeightCount * invObject2->m_nInvWidthCount);
			m_inventoryObjects.Add(invObject2);
		}
	}
}

int32 ASpaceExplorerGameMode::AssignUniqueObjectID()
{
	// return next object ID and then increment it
	return m_nNextObjectID++;
}*/
