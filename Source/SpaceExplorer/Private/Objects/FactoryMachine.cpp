// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "FactoryMachine.h"
#include "Net/UnrealNetwork.h"

AFactoryMachine::AFactoryMachine(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	ProductionRate = 1;
	MinimumProduction = 1;
	CurrentProduction = 0;

	bReplicates = true;
}


void AFactoryMachine::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFactoryMachine, ProductionRate);
	DOREPLIFETIME(AFactoryMachine, MinimumProduction);
	DOREPLIFETIME(AFactoryMachine, CurrentProduction);
}