#include "AEZCharacter.h"
#include "Component/UEZCharacterMovementComponent.h"
#include "Component/UEZStaminaComponent.h"
#include "Component/UEZWeaponPresentationComponent.h"
#include "EchoZone/Interaction/UEZInteractComponent.h"
#include "EchoZone/Character/Health/Components/UEZHealthComponent.h"
#include "EchoZone/Interaction/UEZInteractWidget.h"
#include "EchoZone/Weapon/AEZWeaponBase.h"
#include "EchoZone/Weapon/DataAsset/UEZWeaponDataAsset.h"
#include "EchoZone/UI/UEZHUDWidget.h"

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

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEZCharacter::HandleHealthDeath);
		HealthComponent->OnDropWeaponRequested.AddDynamic(this, &AEZCharacter::HandleDropWeaponRequested);
		HealthComponent->OnScreamRequested.AddDynamic(this, &AEZCharacter::HandleScreamRequested);
		HealthComponent->OnModifiersChanged.AddDynamic(this, &AEZCharacter::HandleHealthModifiersChanged);
	}

	UpdateInteractWidget();
	EquipStarterWeapon();
	SyncWeaponPresentation();

	SyncHealthState();
	ApplyHealthModifiersToCharacter();

	if (HUDWidgetClass)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			HUDWidget = CreateWidget<UEZHUDWidget>(PlayerController, HUDWidgetClass);
			if (HUDWidget)
			{
				HUDWidget->AddToViewport();
				HUDWidget->SetOwnerCharacter(this);
			}
		}
	}
}

void AEZCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFreeLook(DeltaTime);
	UpdateView(DeltaTime);
	UpdateWeaponFOV(DeltaTime);

	SyncHealthState();
	ApplyHealthModifiersToCharacter();
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

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveInput.Y);
	AddMovementInput(RightDirection, MoveInput.X);
}

void AEZCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();

	float SensitivityMultiplier = 1.0f;
	if (HealthComponent)
	{
		SensitivityMultiplier = HealthComponent->GetHealthModifiers().LookSensitivityMultiplier;
	}

	const FVector2D FinalLook = LookAxis * SensitivityMultiplier;

	if (bIsFreeLooking)
	{
		FreeLookYaw = FMath::Clamp(FreeLookYaw + FinalLook.X, -FreeLookYawLimit, FreeLookYawLimit);
		FreeLookPitch = FMath::Clamp(FreeLookPitch + FinalLook.Y, -FreeLookPitchDownLimit, FreeLookPitchUpLimit);
	}
	else
	{
		AddControllerYawInput(FinalLook.X);
		AddControllerPitchInput(FinalLook.Y);
	}
}

void AEZCharacter::StartSprint()
{
	if (HealthComponent && !HealthComponent->GetHealthModifiers().bCanSprint)
	{
		return;
	}

	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(true);
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(true);
	}
}

void AEZCharacter::StopSprint()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
	}
}

void AEZCharacter::StartCrouch()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
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

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
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

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
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

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
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
	if (!CanInteractByHealth())
	{
		return;
	}

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

	if (InteractComponent->HasInteractable() && CanInteractByHealth())
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

	if (HealthComponent)
	{
		CurrentWeapon->ApplyHealthModifiers(HealthComponent->GetHealthModifiers());
	}

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
	if (!CanAimByHealth())
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->StartAim();

		if (!CurrentWeapon->IsAiming())
		{
			bIsWeaponAiming = false;

			if (WeaponPresentationComponent)
			{
				WeaponPresentationComponent->SetAiming(false);
			}
			return;
		}
	}

	bIsWeaponAiming = true;

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

EEZBodyStance AEZCharacter::GetCurrentHealthStance() const
{
	if (bIsCrouched)
	{
		return EEZBodyStance::Crouching;
	}

	return EEZBodyStance::Standing;
}

bool AEZCharacter::CanAimByHealth() const
{
	return HealthComponent ? HealthComponent->GetHealthModifiers().bCanAim : true;
}

bool AEZCharacter::CanInteractByHealth() const
{
	return HealthComponent ? HealthComponent->GetHealthModifiers().bCanInteract : true;
}

void AEZCharacter::SyncHealthState()
{
	if (!HealthComponent)
	{
		return;
	}

	const FVector HorizontalVelocity(GetVelocity().X, GetVelocity().Y, 0.0f);
	const bool bMoving = HorizontalVelocity.SizeSquared() > KINDA_SMALL_NUMBER;

	bool bSprinting = false;
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		bSprinting = EZMoveComp->IsSprintActive();
	}

	HealthComponent->SetMovementStateSnapshot(bMoving, bSprinting, GetCurrentHealthStance());
}

void AEZCharacter::ApplyHealthModifiersToCharacter()
{
	if (!HealthComponent)
	{
		return;
	}

	const FEZHealthModifiers Mods = HealthComponent->GetHealthModifiers();

	if (StaminaComponent)
	{
		StaminaComponent->SetExternalMaxStaminaMultiplier(Mods.MaxStaminaMultiplier);
		StaminaComponent->SetExternalDrainMultiplier(Mods.StaminaDrainMultiplier);
		StaminaComponent->SetExternalRecoveryMultiplier(Mods.StaminaRecoveryMultiplier);
		StaminaComponent->SetStaminaRecoveryBlocked(Mods.bBlockStaminaRecovery);
		StaminaComponent->SetFreeSprint(Mods.bFreeSprint);
	}

	if (InteractComponent)
	{
		InteractComponent->SetInteractionBlocked(!Mods.bCanInteract);
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->ApplyHealthModifiers(Mods);
	}

	if (!Mods.bCanSprint)
	{
		if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
		{
			EZMoveComp->SetSprintIntent(false);
		}

		if (StaminaComponent)
		{
			StaminaComponent->SetSprinting(false);
		}
	}

	if (!Mods.bCanAim && bIsWeaponAiming)
	{
		StopAim();
	}

	UpdateInteractWidget();
}

void AEZCharacter::HandleHealthDeath()
{
	if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
	{
		EZMoveComp->SetSprintIntent(false);
	}

	if (StaminaComponent)
	{
		StaminaComponent->SetSprinting(false);
	}

	StopFire();
	StopAim();

	if (CurrentWeapon)
	{
		CurrentWeapon->StopAllWeaponActions();
	}

	if (InteractComponent)
	{
		InteractComponent->SetInteractionBlocked(true);
	}

	DisableInput(Cast<APlayerController>(GetController()));

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}
}

void AEZCharacter::HandleDropWeaponRequested()
{
	if (!CurrentWeapon)
	{
		return;
	}

	CurrentWeapon->DropFromOwner();
	CurrentWeapon = nullptr;
	bIsWeaponAiming = false;

	SyncWeaponPresentation();
}

void AEZCharacter::HandleScreamRequested()
{
	// Здесь можешь проигрывать звук/анимацию
}

void AEZCharacter::HandleHealthModifiersChanged(const FEZModifiersChangedEvent& EventData)
{
	ApplyHealthModifiersToCharacter();
}