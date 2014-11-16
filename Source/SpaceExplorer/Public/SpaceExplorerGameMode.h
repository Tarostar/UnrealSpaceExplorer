// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

// class AItem;
//#include "ItemFactory.h";
#include "SpaceExplorerGameMode.generated.h"

UCLASS(minimalapi)
class ASpaceExplorerGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

public:
	// TODO: this should probably move to GameState
	// TODO: review
	/* temporary method to create a new item with an unique ID */
	// AItem* CreateItem();

private:
	// TODO: this should probably move to GameState
	// TODO: thought to create an item factory that everything can use to spawn items, probably better than using game mode, but where should it live - in SpaceExplorer?
	//ItemFactory m_itemFactory;
	// this limits us to a lifetime of 2,147,483,647 objects to ever exist
	// not very happy with this method, but use for now until better method is found to uniquely identify an item instance
	// can we simply rely on item pointer... ?
	// int32 m_nNextObjectID;
	// int32 AssignUniqueObjectID();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);
};



