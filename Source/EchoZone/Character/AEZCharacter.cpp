#include "AEZCharacter.h"
#include "Component/UEZCharacterMovementComponent.h"
#include "Component/UEZStaminaComponent.h"
#include "Component/UEZWeaponPresentationComponent.h"
#include "EchoZone/Interaction/UEZInteractComponent.h"
#include "Component/UEZHealthComponent.h"
#include "EchoZone/Interaction/UEZInteractWidget.h"
#include "EchoZone/Weapon/AEZWeaponBase.h"
#include "EchoZone/Weapon/DataAsset/UEZWeaponDataAsset.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

AEZCharacter::AEZCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEZCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	ViewRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewRoot"));
	ViewRootComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(ViewRootComponent);
	CameraComponent->bUsePawnControlRotation = false;

	ADSAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ADSAnchor"));
	ADSAnchor->SetupAttachment(CameraComponent);

	WeaponPresentationComponent = CreateDefaultSubobject<UEZWeaponPresentationComponent>(TEXT("WeaponPresentationComponent"));

	StaminaComponent = CreateDefaultSubobject<UEZStaminaComponent>(TEXT("StaminaComponent"));
	InteractComponent = CreateDefaultSubobject<UEZInteractComponent>(TEXT("InteractComponent"));
	HealthComponent = CreateDefaultSubobject<UEZHealthComponent>(TEXT("HealthComponent"));

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	TargetViewZ = StandingViewZ;
	CurrentWeapon = nullptr;
}

void AEZCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
				{
					SubSystem->AddMappingContext(DefaultMappingContext, 0);
				}
			}
		}
	}

	if (CameraComponent)
	{
		DefaultFOV = CameraComponent->FieldOfView;
	}

	UpdateLeanState();
	UpdateMovementFlags();

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->RefreshMovementSettings();
	}

	if (InteractComponent)
	{
		InteractComponent->OnInteractable.AddDynamic(this, &AEZCharacter::HandleInteractableChanged);
	}

	if (InteractWidgetClass)
	{
		InteractWidget = CreateWidget<UEZInteractWidget>(GetWorld(), InteractWidgetClass);
		if (InteractWidget)
		{
			InteractWidget->AddToViewport();
			InteractWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	UpdateInteractWidget();
	EquipStarterWeapon();
	SyncWeaponPresentation();
}

void AEZCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFreeLook(DeltaTime);
	UpdateView(DeltaTime);
	UpdateWeaponFOV(DeltaTime);
}

void AEZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEZCharacter::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AEZCharacter::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AEZCharacter::Move);
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
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AEZCharacter::StartCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AEZCharacter::StopCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopCrouch);
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

		if (Interacted)
		{
			EnhancedInputComponent->BindAction(Interacted, ETriggerEvent::Started, this, &AEZCharacter::Interact);
		}

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AEZCharacter::StartFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AEZCharacter::StopFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopFire);
		}

		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AEZCharacter::ReloadWeapon);
		}

		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AEZCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AEZCharacter::StopAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopAim);
		}

		if (SwitchFireModeAction)
		{
			EnhancedInputComponent->BindAction(SwitchFireModeAction, ETriggerEvent::Started, this, &AEZCharacter::SwitchFireMode);
		}
	}
}

UEZCharacterMovementComponent* AEZCharacter::GetUEZMovementComponent() const
{
	return Cast<UEZCharacterMovementComponent>(GetCharacterMovement());
}

void AEZCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetMoveInput(MoveInput);
	}

	if (!Controller)
	{
		return;
	}

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxes(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxes(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveInput.Y);
	AddMovementInput(RightDirection, MoveInput.X);
}

void AEZCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	if (bIsFreeLooking)
	{
		FreeLookYaw = FMath::Clamp(FreeLookYaw + LookAxis.X, -FreeLookYawLimit, FreeLookYawLimit);
		FreeLookPitch = FMath::Clamp(FreeLookPitch + LookAxis.Y, -FreeLookPitchDownLimit, FreeLookPitchUpLimit);
	}
	else
	{
		AddControllerYawInput(LookAxis.X);
		AddControllerPitchInput(LookAxis.Y);
	}
}

void AEZCharacter::StartSprint()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(true);
	}
}

void AEZCharacter::StopSprint()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}
}

void AEZCharacter::StartCrouch()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	if (!bIsCrouched)
	{
		Crouch();
	}
}

void AEZCharacter::StopCrouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AEZCharacter::StartLeanLeft()
{
	bIsLeaningLeft = true;
	bIsLeaningRight = false;

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	UpdateLeanState();
	UpdateMovementFlags();
}

void AEZCharacter::StopLeanLeft()
{
	bIsLeaningLeft = false;
	UpdateLeanState();
	UpdateMovementFlags();
}

void AEZCharacter::StartLeanRight()
{
	bIsLeaningRight = true;
	bIsLeaningLeft = false;

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	UpdateLeanState();
	UpdateMovementFlags();
}

void AEZCharacter::StopLeanRight()
{
	bIsLeaningRight = false;
	UpdateLeanState();
	UpdateMovementFlags();
}

void AEZCharacter::StartFreeLook()
{
	bIsFreeLooking = true;

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	UpdateMovementFlags();
}

void AEZCharacter::StopFreeLook()
{
	bIsFreeLooking = false;
	UpdateMovementFlags();
}

void AEZCharacter::IncreaseWalkSpeedStep()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->IncreaseWalkSpeedStep();
	}
}

void AEZCharacter::DecreaseWalkSpeedStep()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->DecreaseWalkSpeedStep();
	}
}

void AEZCharacter::Interact()
{
	if (InteractComponent)
	{
		InteractComponent->TryInteract();
		UpdateInteractWidget();
	}
}

void AEZCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	TargetViewZ = CrouchedViewZ;

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetLocomotionStance(ELocomotionStance::Crouched);
		EZMoveComp->RefreshMovementSettings();
	}
}

void AEZCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	TargetViewZ = StandingViewZ;

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetLocomotionStance(ELocomotionStance::Standing);
		EZMoveComp->RefreshMovementSettings();
	}
}

void AEZCharacter::UpdateLeanState()
{
	if (bIsLeaningLeft && !bIsLeaningRight)
	{
		TargetLeanRoll = -LeanAngle;
		TargetLeanOffsetY = -LeanOffset;
	}
	else if (bIsLeaningRight && !bIsLeaningLeft)
	{
		TargetLeanRoll = LeanAngle;
		TargetLeanOffsetY = LeanOffset;
	}
	else
	{
		TargetLeanRoll = 0.0f;
		TargetLeanOffsetY = 0.0f;
	}
}

void AEZCharacter::UpdateMovementFlags()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetMovementBlockFlag(EMovementBlockFlags::FreeLook, bIsFreeLooking);
		EZMoveComp->SetMovementBlockFlag(EMovementBlockFlags::Lean, bIsLeaningLeft || bIsLeaningRight);
	}
}

void AEZCharacter::UpdateView(float DeltaTime)
{
	if (!ViewRootComponent || !GetMesh())
	{
		return;
	}

	const FTransform HeadSocketTransform = GetMesh()->GetSocketTransform(FirstPersonCameraSocketName, RTS_World);

	FVector TargetWorldLocation = HeadSocketTransform.GetLocation();
	TargetWorldLocation += GetActorRightVector() * TargetLeanOffsetY;

	FRotator ControlRot = Controller ? Controller->GetControlRotation() : GetActorRotation();

	FRotator TargetWorldRotation = ControlRot;
	TargetWorldRotation.Roll += TargetLeanRoll;
	TargetWorldRotation.Yaw += FreeLookYaw;

	if (bIsFreeLooking)
	{
		TargetWorldRotation.Pitch += FreeLookPitch;
	}

	const FVector CurrentLocation = ViewRootComponent->GetComponentLocation();
	const FRotator CurrentRotation = ViewRootComponent->GetComponentRotation();

	const FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetWorldLocation, DeltaTime, ViewHeightInterpSpeed);
	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetWorldRotation, DeltaTime, LeanInterpSpeed);

	ViewRootComponent->SetWorldLocation(NewLocation);
	ViewRootComponent->SetWorldRotation(NewRotation);
}

void AEZCharacter::UpdateFreeLook(float DeltaTime)
{
	if (bIsFreeLooking)
	{
		return;
	}

	FreeLookYaw = FMath::FInterpTo(FreeLookYaw, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
	FreeLookPitch = FMath::FInterpTo(FreeLookPitch, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
}

void AEZCharacter::UpdateWeaponFOV(float DeltaTime)
{
	if (!CameraComponent)
	{
		return;
	}

	float TargetFOV = DefaultFOV;
	float InterpSpeed = FOVInterpSpeedFallback;

	if (CurrentWeapon && CurrentWeapon->GetWeaponData())
	{
		const UEZWeaponDataAsset* WeaponData = CurrentWeapon->GetWeaponData();
		TargetFOV = bIsWeaponAiming ? WeaponData->ADS.AimFOV : DefaultFOV;
		InterpSpeed = bIsWeaponAiming
			? (1.0f / FMath::Max(0.01f, WeaponData->ADS.EnterTime))
			: (1.0f / FMath::Max(0.01f, WeaponData->ADS.ExitTime));
	}

	const float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, InterpSpeed);
	CameraComponent->SetFieldOfView(NewFOV);
}

void AEZCharacter::SyncWeaponPresentation()
{
	if (!WeaponPresentationComponent)
	{
		return;
	}

	WeaponPresentationComponent->SetCurrentWeapon(CurrentWeapon);
	WeaponPresentationComponent->SetADSAnchor(ADSAnchor);
	WeaponPresentationComponent->SetAiming(bIsWeaponAiming);
}

void AEZCharacter::HandleInteractableChanged(AActor* NewInteractable)
{
	UpdateInteractWidget();
}

void AEZCharacter::UpdateInteractWidget()
{
	if (!InteractWidget || !InteractComponent)
	{
		return;
	}

	if (InteractComponent->HasInteractable())
	{
		const FText ActionText = InteractComponent->GetCurrentInteractText();
		const FText FinalText = FText::Format(FText::FromString(TEXT("[F] {0}")), ActionText);

		InteractWidget->SetInteractText(FinalText);
		InteractWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AEZCharacter::EquipStarterWeapon()
{
	if (!StarterWeaponClass || !GetWorld() || !GetMesh())
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AEZWeaponBase>(StarterWeaponClass, SpawnParams);
	if (!CurrentWeapon)
	{
		return;
	}

	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);

	SyncWeaponPresentation();
}

void AEZCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AEZCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AEZCharacter::ReloadWeapon()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Reload();
	}
}

void AEZCharacter::StartAim()
{
	bIsWeaponAiming = true;

	if (CurrentWeapon)
	{
		CurrentWeapon->StartAim();
	}

	if (WeaponPresentationComponent)
	{
		WeaponPresentationComponent->SetAiming(true);
	}
}

void AEZCharacter::StopAim()
{
	bIsWeaponAiming = false;

	if (CurrentWeapon)
	{
		CurrentWeapon->StopAim();
	}

	if (WeaponPresentationComponent)
	{
		WeaponPresentationComponent->SetAiming(false);
	}
}

void AEZCharacter::SwitchFireMode()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SwitchFireMode();
	}
}