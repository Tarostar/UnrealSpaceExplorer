// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "CustomObject.h"
#include "Net/UnrealNetwork.h"


ACustomObject::ACustomObject(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Name = "none";
	Type = "";
	bNotifyFactions = true;
	ProductionCost = 1;

	bReplicates = true;
}

void ACustomObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomObject, Name);
	DOREPLIFETIME(ACustomObject, bNotifyFactions);
	DOREPLIFETIME(ACustomObject, ProductionCost);
	DOREPLIFETIME(ACustomObject, Type);
}

void ACustomObject::SaveLoad(FArchive& Ar)
{
	Ar << Name;
	Ar << Type;
	Ar << bNotifyFactions;
	Ar << ProductionCost;

}
