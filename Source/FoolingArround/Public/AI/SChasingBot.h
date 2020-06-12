// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SChasingBot.generated.h"

UCLASS()
class FOOLINGARROUND_API ASChasingBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASChasingBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	FVector FindNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "ChasingBot")
	float MovmentForce;

	UPROPERTY(EditDefaultsOnly, Category = "ChasingBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "ChasingBot")
	float RequiredDistanceToTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
