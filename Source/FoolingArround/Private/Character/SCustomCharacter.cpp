// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SCustomCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ASCustomCharacter::ASCustomCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ZoomedFOV = 65.0f;
	ZoomingSpeed = 20;

}

// Called when the game starts or when spawned
void ASCustomCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = FollowCamera->FieldOfView;
}

void ASCustomCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASCustomCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(GetActorForwardVector() * Value);
	}
}

void ASCustomCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		AddMovementInput(GetActorRightVector() * Value);
	}
}

void ASCustomCharacter::Jump()
{
	bIsJumping = true;
}

void ASCustomCharacter::EndJump()
{
	bIsJumping = false;
}

void ASCustomCharacter::BeginCrouch()
{
	Crouch();
}

void ASCustomCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCustomCharacter::BeginZoom()
{
	bIsZooming = true;
}

void ASCustomCharacter::EndZoom()
{
	bIsZooming = false;
}

void ASCustomCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASCustomCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASCustomCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ASCustomCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

// Called every frame
void ASCustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bIsZooming ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomingSpeed);

	FollowCamera->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	/*PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCustomCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCustomCharacter::EndJump);*/

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCustomCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCustomCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &ASCustomCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCustomCharacter::AddControllerPitchInput);


	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCustomCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCustomCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCustomCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCustomCharacter::EndZoom);

}

FVector ASCustomCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetComponentLocation();
	}
	// fallback if the if-clause fails
	return Super::GetPawnViewLocation();
}

