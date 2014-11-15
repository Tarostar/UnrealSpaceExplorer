// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "ObjectTemplate.h"
#include "Net/UnrealNetwork.h"


AObjectTemplate::AObjectTemplate(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bNotifyFactory = true;
	ProductionCost = 1;
	Type = "";

	bReplicates = true;
}

void AObjectTemplate::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObjectTemplate, bNotifyFactions);
	DOREPLIFETIME(AObjectTemplate, ProductionCost);
	DOREPLIFETIME(AObjectTemplate, Type);
}

void AObjectTemplate::SaveLoad(FArchive& Ar)
{
	Ar << bNotifyFactions;
	Ar << ProductionCost;
	Ar << Type;

}
