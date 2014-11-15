// Fill out your copyright notice in the Description page of Project Settings.

#include "SpaceExplorer.h"
#include "ObjectFactory.h"
#include "Net/UnrealNetwork.h"


AObjectFactory::AObjectFactory(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bActive = true;
	bNotifyFactions = true;
	FactoryIntegrity = 100;

	bReplicates = true;
}

void AObjectFactory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AObjectFactory, DeployPortals);
	DOREPLIFETIME(AObjectFactory, Machines);
	//DOREPLIFETIME(AObjectFactory, FactionOwners);
	
	DOREPLIFETIME(AObjectFactory, bActive);
	DOREPLIFETIME(AObjectFactory, bNotifyFactions);
	DOREPLIFETIME(AObjectFactory, FactoryIntegrity);
}

void AObjectFactory::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	DeployObject();
}

void AObjectFactory::PopulateQueue()
{

}

void AObjectFactory::DeployObject()
{
	// TODO: all the logic of deployment
	/*
	static OffsetsGeneratorHelper OffsetsGenerator;

	const bool bShoudSpawnNewUnits = GetWorld()->GetTimeSeconds() > NextSpawnTime;
	if (!bShoudSpawnNewUnits)
	{
		return;
	}

	if (EnemyBrewery == NULL)
	{
		const EStrategyTeam::Type EnemyTeamNum = (MyTeamNum == EStrategyTeam::Player ? EStrategyTeam::Enemy : EStrategyTeam::Player);
		const FPlayerData* const EnemyTeamData = GetWorld()->GetGameState<AStrategyGameState>()->GetPlayerData(EnemyTeamNum);
		if (EnemyTeamData != NULL && EnemyTeamData->Brewery != NULL)
		{
			EnemyBrewery = EnemyTeamData->Brewery;
		}
	}

	if (WaveSize > 0)
	{
		// find best place on ground to spawn at
		const AStrategyBuilding_Brewery* const Owner = Cast<AStrategyBuilding_Brewery>(GetOwner());
		check(Owner);
		bool bSpawnedNewMinion = false;
		if (Owner->MinionCharClass != NULL)
		{
			FVector Loc = Owner->GetActorLocation();
			const FVector X = Owner->GetTransform().GetScaledAxis(EAxis::X);
			const FVector Y = Owner->GetTransform().GetScaledAxis(EAxis::Y);
			Loc += X * RadiusToSpawnOn + Y * OffsetsGenerator.GetOffset();

			const FVector Scale(CustomScale);
			const FVector TraceOffset(0.0f, 0.0f, RadiusToSpawnOn * 0.5 * Scale.Z);
			FHitResult Hit;
			GetWorld()->LineTraceSingle(Hit, Loc + TraceOffset, Loc - TraceOffset, FCollisionQueryParams(NAME_None, true, Owner), FCollisionObjectQueryParams(ECC_WorldStatic));
			if (Hit.Actor.IsValid())
			{
				Loc = Hit.Location + FVector(0.0f, 0.0f, Scale.Z * 10.0f);
			}
			AStrategyChar* StrategyChar = Owner->MinionCharClass->GetDefaultObject<AStrategyChar>();
			const float CapsuleHalfHeight = StrategyChar->CapsuleComponent->GetUnscaledCapsuleHalfHeight();
			const float CapsuleRadius = StrategyChar->CapsuleComponent->GetUnscaledCapsuleRadius();
			Loc = Loc + FVector(0.0f, 0.0f, Scale.Z * CapsuleHalfHeight);

			// and spawn our minion
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.bNoCollisionFail = true;
			AStrategyChar* const MinionChar = GetWorld()->SpawnActor<AStrategyChar>(Owner->MinionCharClass, Loc, Owner->GetActorRotation(), SpawnInfo);
			// don't continue if he died right away on spawn
			if ((MinionChar != NULL) && (MinionChar->bIsDying == false))
			{
				// Flag a successful spawn
				bSpawnedNewMinion = true;

				MinionChar->SetTeamNum(GetTeamNum());

				MinionChar->SpawnDefaultController();
				MinionChar->CapsuleComponent->SetRelativeScale3D(Scale);
				MinionChar->CapsuleComponent->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
				MinionChar->Mesh->GlobalAnimRateScale = AnimationRate;

				AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
				if (GameState != NULL)
				{
					GameState->OnCharSpawned(MinionChar);
				}

				MinionChar->ApplyBuff(BuffModifier);
				if (DefaultWeapon != NULL)
				{
					UStrategyGameBlueprintLibrary::GiveWeaponFromClass(MinionChar, DefaultWeapon);
				}
				if (DefaultArmor != NULL)
				{
					UStrategyGameBlueprintLibrary::GiveArmorFromClass(MinionChar, DefaultArmor);
				}

				WaveSize -= 1;
				WaveSize = FMath::Max(WaveSize, 0);
				if (Owner != NULL && WaveSize <= 0 && MyTeamNum == EStrategyTeam::Enemy)
				{
					Owner->OnWaveSpawned.Broadcast();
				}
				NextSpawnTime = GetWorld()->GetTimeSeconds() + FMath::FRandRange(2.0f, 3.0f);
			}
			else
			{
				UE_LOG(LogGame, Warning, TEXT("Failed to spawn minion."));
			}
		}
		else
		{
			// If we dont have a class type we cannot spawn a minion. 
			UE_LOG(LogGame, Warning, TEXT("No minion class specified in %s. Cannot spawn minion"), *Owner->GetName());
		}
		// If we failed to spawn a minion try again soon
		if (bSpawnedNewMinion == false)
		{
			NextSpawnTime = GetWorld()->GetTimeSeconds() + 0.1f;
		}
	}*/
}


