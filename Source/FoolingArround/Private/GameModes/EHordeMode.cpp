// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/EHordeMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Components/SHealthComponent.h"

AEHordeMode::AEHordeMode()
{
	TimeBetweenWaves = 2.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AEHordeMode::StartWave()
{
	WaveCount++;

	NumberOfEnemiesToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AEHordeMode::SpawnInTimeElapsed, 1.0f, true, 0.0f);
}

void AEHordeMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	//PrepareForNextWave();
}

void AEHordeMode::SpawnInTimeElapsed()
{
	SpawnBot();

	NumberOfEnemiesToSpawn--;

	if (NumberOfEnemiesToSpawn <= 0)
	{
		EndWave();
	}
}

void AEHordeMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AEHordeMode::StartWave, false);
}

void AEHordeMode::CheckWaveState()
{
	bool bIsPrepearingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NumberOfEnemiesToSpawn > 0 || bIsPrepearingForWave)
	{
		return; 
	}

	bool bAreEnemiesDead = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* TestPawn = It->Get(); 
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}
		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bAreEnemiesDead = true;
			break;
		}
	}

	if (!bAreEnemiesDead)
	{
		PrepareForNextWave();
	}
}

void AEHordeMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AEHordeMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}
