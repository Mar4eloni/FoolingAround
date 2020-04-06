// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/SWeaponBase.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASWeaponBase::ASWeaponBase()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "BeamEnd";
}

// Called when the game starts or when spawned
void ASWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

void ASWeaponBase::Fire()//(FName WeaponSocketName, USkeletalMeshComponent* CharacterMesh)
{
	// Trace from player eyes to crosshair

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		// we get the correct location and rotation with this function
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		//FVector WeaponLocation = CharacterMesh->GetSocketLocation(WeaponSocketName);
		//EyeRotation = CharacterMesh->GetSocketRotation(WeaponSocketName);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;

		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// make damage

			AActor* HitActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			UE_LOG(LogTemp, Log, TEXT("HIT !!!"));

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Green, false, 1.0f, 0, 1.0f);
		
		PlayShootingEffects(TracerEndPoint);
	}
}

void ASWeaponBase::PlayShootingEffects(FVector TraceEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		ShakeCamera();
	}
	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (TracerComp)
		{
			TracerComp->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}
}

void ASWeaponBase::ShakeCamera()
{
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}


