// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "FactoryPortal.h"
#include "Net/UnrealNetwork.h"


AFactoryPortal::AFactoryPortal(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bActive = true;
	Position = FVector(0.f, 0.f, 0.f);
	DeployRadius = 0;
	Priority = 1;
	DeployInventory = nullptr;
	bOnlyDeployToInventory = false;

	bReplicates = true;
}

void AFactoryPortal::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFactoryPortal, bActive);
	DOREPLIFETIME(AFactoryPortal, Position);
	DOREPLIFETIME(AFactoryPortal, DeployRadius);
	DOREPLIFETIME(AFactoryPortal, Priority);
	DOREPLIFETIME(AFactoryPortal, DeployInventory);
	DOREPLIFETIME(AFactoryPortal, bOnlyDeployToInventory);
}
