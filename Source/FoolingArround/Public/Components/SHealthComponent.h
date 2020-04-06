// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"


class AController;

// On Health Change Event
//DECLARE_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, USHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOOLINGARROUND_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponents")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthComponents")
	float DefaultHealth;
	
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

public:

	/*UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangeSignature* OnHealthChanged;*/

};
