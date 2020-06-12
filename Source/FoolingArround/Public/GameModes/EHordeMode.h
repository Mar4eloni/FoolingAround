// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EHordeMode.generated.h"

/**
 * 
 */
UCLASS()
class FOOLINGARROUND_API AEHordeMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;
	
	int32 NumberOfEnemiesToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnBot();

	void StartWave();

	void EndWave();

	void SpawnInTimeElapsed();

	// timer for new wave
	void PrepareForNextWave();

	void CheckWaveState();
	
public:

	AEHordeMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
