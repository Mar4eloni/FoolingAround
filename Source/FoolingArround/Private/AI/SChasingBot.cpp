// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SChasingBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

// Sets default values
ASChasingBot::ASChasingBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false); // bCanEverAffectNavigation = false;
}

// Called when the game starts or when spawned
void ASChasingBot::BeginPlay()
{
	Super::BeginPlay();
	
}

//FVector ASChasingBot::FindNextPathPoint()
//{
//	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
//	//UNavigationSystemBase:
//	//PlayerPawn->getactor
//	
//	//UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
//
//	//if (NavPath->PathPoints.Num() > 1)
//	//{
//	//	//Return next point in path
//	//	return NavPath->PathPoints[1];
//	//}
//	//// if failed to find path
//	//return GetActorLocation();
//	return new FVector();
//}

FVector ASChasingBot::FindNextPathPoint()
{
	return FVector();
}

// Called every frame
void ASChasingBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASChasingBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

}

