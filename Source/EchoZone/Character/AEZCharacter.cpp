// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
AEZCharacter::AEZCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	ViewRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewRoot"));
	ViewRootComponent->SetupAttachment(SpringArmComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	GetCharacterMovement()->MaxAcceleration = MovementAcceleration;
	GetCharacterMovement()->BrakingDecelerationWalking = MovementDeceleration;
	GetCharacterMovement()->GroundFriction = GroundFriction;

	CurrentWalkSpeedStepIndex = DefaultWalkSpeedStepIndex;
	TargetViewZ = StandingViewZ;
}

// Called when the game starts or when spawned
void AEZCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
				{
					InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}
	
	if (ViewRootComponent)
	{
		FVector RelativeLocation = ViewRootComponent->GetRelativeLocation();
		RelativeLocation.Z = StandingViewZ;
		ViewRootComponent->SetRelativeLocation(RelativeLocation);
	}

	UpdateMovementSettings();
}

void AEZCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFreeLook(DeltaTime);
	UpdateView(DeltaTime);
	UpdateMovementSettings();
}


// Called to bind functionality to input
void AEZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEZCharacter::Move);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEZCharacter::Look);
		}
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AEZCharacter::StartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AEZCharacter::StopSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopSprint);
		}
		if (CrouchAction)
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AEZCharacter::ToggleCrouch);
		}
		if (LeanLeftAction)
		{
			EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &AEZCharacter::StartLeanLeft);
			EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Completed, this, &AEZCharacter::StopLeanLeft);
			EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopLeanLeft);
		}
		if (LeanRightAction)
		{
			EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Started, this, &AEZCharacter::StartLeanRight);
			EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Completed, this, &AEZCharacter::StopLeanRight);
			EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopLeanRight);
		}
		if (FreeLookAction)
		{
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &AEZCharacter::StartFreeLook);
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &AEZCharacter::StopFreeLook);
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopFreeLook);
		}
		if (WalkSpeedUpAction)
		{
			EnhancedInputComponent->BindAction(WalkSpeedUpAction, ETriggerEvent::Started, this, &AEZCharacter::IncreaseWalkSpeedStep);
		}
		if (WalkSpeedDownAction)
		{
			EnhancedInputComponent->BindAction(WalkSpeedDownAction, ETriggerEvent::Started, this, &AEZCharacter::DecreaseWalkSpeedStep);
		}
	}
}

void AEZCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AEZCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (bIsFreeLooking)
	{
		FreeLookYaw = FMath::Clamp(FreeLookYaw + LookAxisVector.X, -FreeLookYawLimit, FreeLookYawLimit);
		FreeLookPitch = FMath::Clamp(FreeLookPitch + LookAxisVector.Y, -FreeLookPitchDownLimit, FreeLookPitchUpLimit);
		CancelSprint();
	}
	else
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AEZCharacter::StartSprint()
{
	if (!CanSprint())
	{
		return;
	}
	bWantsToSprint = true;
	UpdateMovementSettings();
}

void AEZCharacter::StopSprint()
{
	bWantsToSprint = false;
	UpdateMovementSettings();
}

void AEZCharacter::ToggleCrouch()
{
	CancelSprint();

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}

	UpdateMovementSettings();
}

void AEZCharacter::StartLeanLeft()
{
	if (bWantsToSprint)
	{
		CancelSprint();
	}

	TargetLeanRoll = -LeanAngle;
	TargetLeanOffsetY = -LeanOffset;
}

void AEZCharacter::StopLeanLeft()
{
	if (TargetLeanRoll < 0.0f)
	{
		TargetLeanRoll = 0.0f;
	}

	if (TargetLeanOffsetY < 0.0f)
	{
		TargetLeanOffsetY = 0.0f;
	}
}

void AEZCharacter::StartLeanRight()
{
	if (bWantsToSprint)
	{
		CancelSprint();
	}

	TargetLeanRoll = LeanAngle;
	TargetLeanOffsetY = LeanOffset;
}

void AEZCharacter::StopLeanRight()
{
	if (TargetLeanRoll > 0.0f)
	{
		TargetLeanRoll = 0.0f;
	}
	if (TargetLeanOffsetY > 0.0f)
	{
		TargetLeanOffsetY = 0.0f;
	}
}

void AEZCharacter::StartFreeLook()
{
	bIsFreeLooking = true;
	CancelSprint();
}

void AEZCharacter::StopFreeLook()
{
	bIsFreeLooking = false;
}

void AEZCharacter::IncreaseWalkSpeedStep()
{
	if (WalkSpeedSteps.Num() == 0)
	{
		return;
	}

	CurrentWalkSpeedStepIndex = FMath::Clamp(CurrentWalkSpeedStepIndex + 1, 0, WalkSpeedSteps.Num() - 1);
	UpdateMovementSettings();
}

void AEZCharacter::DecreaseWalkSpeedStep()
{
	if (WalkSpeedSteps.Num() == 0)
	{
		return;
	}

	CurrentWalkSpeedStepIndex = FMath::Clamp(CurrentWalkSpeedStepIndex - 1, 0, WalkSpeedSteps.Num() - 1);
	UpdateMovementSettings();
}

