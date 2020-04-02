// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SWeaponBase.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"

// Sets default values
ASWeaponBase::ASWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

}

// Called when the game starts or when spawned
void ASWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeaponBase::Fire()
{
	// Trace from player eyes to crosshair

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		// we get the correct location and rotation with this function
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// make damage
		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
	}
}

// Called every frame
void ASWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

