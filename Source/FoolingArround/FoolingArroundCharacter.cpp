// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FoolingArroundCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"

//////////////////////////////////////////////////////////////////////////
// AFoolingArroundCharacter

AFoolingArroundCharacter::AFoolingArroundCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;


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

//////////////////////////////////////////////////////////////////////////
// Input

void AFoolingArroundCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AFoolingArroundCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &AFoolingArroundCharacter::EndJump);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFoolingArroundCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFoolingArroundCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AFoolingArroundCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFoolingArroundCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AFoolingArroundCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFoolingArroundCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AFoolingArroundCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AFoolingArroundCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFoolingArroundCharacter::OnResetVR);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFoolingArroundCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AFoolingArroundCharacter::EndCrouch );

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFoolingArroundCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AFoolingArroundCharacter::EndZoom);

}

FVector AFoolingArroundCharacter::GetPawnViewLocation() const
{
	if (FollowCamera)
	{
		return FollowCamera->GetComponentLocation();
	}
	// fallback if the if-clause fails
	return Super::GetPawnViewLocation();
}

void AFoolingArroundCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bIsZooming ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomingSpeed);

	FollowCamera->SetFieldOfView(NewFOV);
}

void AFoolingArroundCharacter::BeginPlay()
{
	
	Super::BeginPlay();



	DefaultFOV = FollowCamera->FieldOfView;

}


void AFoolingArroundCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFoolingArroundCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AFoolingArroundCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AFoolingArroundCharacter::BeginZoom()
{
	bIsZooming = true;
}

void AFoolingArroundCharacter::EndZoom()
{
	bIsZooming = false;
}

void AFoolingArroundCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFoolingArroundCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFoolingArroundCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AFoolingArroundCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AFoolingArroundCharacter::Jump()
{
	bIsJumping = true;
	//Jump();
}

void AFoolingArroundCharacter::EndJump()
{
	bIsJumping = false;
	//StopJumping();
}

void AFoolingArroundCharacter::BeginCrouch()
{
	Crouch();
}

void AFoolingArroundCharacter::EndCrouch()
{
	UnCrouch();
}
