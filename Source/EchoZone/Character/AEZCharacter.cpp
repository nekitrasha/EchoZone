#include "AEZCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

AEZCharacter::AEZCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 0.0f;
    SpringArmComponent->bUsePawnControlRotation = true;

    ViewRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewRoot"));
    ViewRootComponent->SetupAttachment(SpringArmComponent);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(ViewRootComponent);
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

void AEZCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, 0);
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
    }
}

void AEZCharacter::Move(const FInputActionValue& Value)
{
    CurrentMoveInput = Value.Get<FVector2D>();

    if (!Controller)
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, CurrentMoveInput.Y);
    AddMovementInput(RightDirection, CurrentMoveInput.X);
}

void AEZCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();

    if (bIsFreeLooking)
    {
        FreeLookYaw = FMath::Clamp(FreeLookYaw + LookAxis.X, -FreeLookYawLimit, FreeLookYawLimit);
        FreeLookPitch = FMath::Clamp(FreeLookPitch + LookAxis.Y, -FreeLookPitchDownLimit, FreeLookPitchUpLimit);
        CancelSprint();
    }
    else
    {
        AddControllerYawInput(LookAxis.X);
        AddControllerPitchInput(LookAxis.Y);
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

void AEZCharacter::StartCrouch()
{
    CancelSprint();

    if (!bIsCrouched)
    {
        Crouch();
        UpdateMovementSettings();
    }
}

void AEZCharacter::StopCrouch()
{
    if (bIsCrouched)
    {
        UnCrouch();
        UpdateMovementSettings();
    }
}

void AEZCharacter::StartLeanLeft()
{
    CancelSprint();

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
    CancelSprint();

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

void AEZCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
    TargetViewZ = CrouchedViewZ;
}

void AEZCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
    TargetViewZ = StandingViewZ;
}

float AEZCharacter::GetCurrentWalkStepMultiplier() const
{
    if (WalkSpeedSteps.IsValidIndex(CurrentWalkSpeedStepIndex))
    {
        return WalkSpeedSteps[CurrentWalkSpeedStepIndex];
    }

    return 1.0f;
}

bool AEZCharacter::IsTryingToMoveForwardOnly() const
{
    const bool bForward = CurrentMoveInput.Y > 0.1f;
    const bool bSide = FMath::Abs(CurrentMoveInput.X) > 0.1f;
    const bool bBackward = CurrentMoveInput.Y < -0.1f;

    return bForward && !bSide && !bBackward;
}

float AEZCharacter::GetCurrentGroundAngleDegrees() const
{
    const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp || !MoveComp->IsMovingOnGround())
    {
        return 0.0f;
    }

    const FFindFloorResult& FloorResult = MoveComp->CurrentFloor;
    if (!FloorResult.bBlockingHit)
    {
        return 0.0f;
    }

    const FVector FloorNormal = FloorResult.HitResult.ImpactNormal.GetSafeNormal();
    if (FloorNormal.IsNearlyZero())
    {
        return 0.0f;
    }

    const float DotUp = FVector::DotProduct(FloorNormal, FVector::UpVector);
    return FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotUp, -1.0f, 1.0f)));
}

bool AEZCharacter::CanSprint() const
{
    if (bIsCrouched)
    {
        return false;
    }

    if (bIsFreeLooking)
    {
        return false;
    }

    const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp || !MoveComp->IsMovingOnGround())
    {
        return false;
    }

    if (!IsTryingToMoveForwardOnly())
    {
        return false;
    }

    if (SprintMaxAllowedSlopeAngle > 0.0f)
    {
        if (GetCurrentGroundAngleDegrees() > SprintMaxAllowedSlopeAngle)
        {
            return false;
        }
    }

    return true;
}

void AEZCharacter::CancelSprint()
{
    if (bWantsToSprint)
    {
        bWantsToSprint = false;
        UpdateMovementSettings();
    }
}

float AEZCharacter::CalculateSlopeSpeedMultiplier() const
{
    if (!bUseSlopeSpeedModifier)
    {
        return 1.0f;
    }

    const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp || !MoveComp->IsMovingOnGround())
    {
        return 1.0f;
    }

    if (CurrentMoveInput.IsNearlyZero())
    {
        return 1.0f;
    }

    const FFindFloorResult& FloorResult = MoveComp->CurrentFloor;
    if (!FloorResult.bBlockingHit)
    {
        return 1.0f;
    }

    const FVector FloorNormal = FloorResult.HitResult.ImpactNormal.GetSafeNormal();
    if (FloorNormal.IsNearlyZero())
    {
        return 1.0f;
    }

    const float DotUp = FVector::DotProduct(FloorNormal, FVector::UpVector);
    const float GroundAngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotUp, -1.0f, 1.0f)));

    if (GroundAngleDeg <= SlopeEffectDeadZoneAngle)
    {
        return 1.0f;
    }

    const FVector Forward = GetActorForwardVector();
    const FVector Right = GetActorRightVector();

    FVector DesiredMoveDirection = (Forward * CurrentMoveInput.Y + Right * CurrentMoveInput.X);
    DesiredMoveDirection.Z = 0.0f;
    DesiredMoveDirection = DesiredMoveDirection.GetSafeNormal();

    if (DesiredMoveDirection.IsNearlyZero())
    {
        return 1.0f;
    }

    FVector DownhillDirection = FVector::VectorPlaneProject(FVector::DownVector, FloorNormal);
    DownhillDirection.Z = 0.0f;
    DownhillDirection = DownhillDirection.GetSafeNormal();

    if (DownhillDirection.IsNearlyZero())
    {
        return 1.0f;
    }

    // +1 = вниз, -1 = вверх, 0 = поперек
    const float AlignmentToDownhill = FVector::DotProduct(DesiredMoveDirection, DownhillDirection);

    // Чем ближе движение к чистому вверх/вниз, тем сильнее влияние уклона
    const float DirectionInfluence = FMath::Lerp(CrossSlopeInfluence, 1.0f, FMath::Abs(AlignmentToDownhill));

    // Движение вверх
    if (AlignmentToDownhill < -0.05f)
    {
        const bool bSprintActive = bWantsToSprint && CanSprint();

        const float MaxAngle = bSprintActive ? MaxUphillAngleSprint : MaxUphillAngleWalk;
        const float MinMultiplier = bSprintActive ? MinSprintUphillSpeedMultiplier : MinWalkUphillSpeedMultiplier;

        const float AngleAlpha = FMath::Clamp(
            (GroundAngleDeg - SlopeEffectDeadZoneAngle) / FMath::Max(MaxAngle - SlopeEffectDeadZoneAngle, 1.0f),
            0.0f,
            1.0f
        );

        const float UphillMultiplier = FMath::Lerp(1.0f, MinMultiplier, AngleAlpha);
        const float FinalAlpha = AngleAlpha * DirectionInfluence;

        return FMath::Lerp(1.0f, UphillMultiplier, FinalAlpha);
    }

    // Движение вниз
    if (AlignmentToDownhill > 0.05f)
    {
        const float AngleAlpha = FMath::Clamp(
            (GroundAngleDeg - SlopeEffectDeadZoneAngle) / FMath::Max(MaxDownhillAngle - SlopeEffectDeadZoneAngle, 1.0f),
            0.0f,
            1.0f
        );

        const float DownhillMultiplier = FMath::Lerp(1.0f, MaxDownhillSpeedMultiplier, AngleAlpha);
        const float FinalAlpha = AngleAlpha * DirectionInfluence;

        return FMath::Lerp(1.0f, DownhillMultiplier, FinalAlpha);
    }

    return 1.0f;
}

float AEZCharacter::CalculateCurrentMaxSpeed() const
{
    float BaseSpeed = ForwardWalkSpeed * GetCurrentWalkStepMultiplier();

    const bool bForward = CurrentMoveInput.Y > 0.1f;
    const bool bSide = FMath::Abs(CurrentMoveInput.X) > 0.1f;
    const bool bBackward = CurrentMoveInput.Y < -0.1f;

    if (bWantsToSprint && CanSprint())
    {
        BaseSpeed = SprintForwardSpeed;
    }

    if (bIsCrouched)
    {
        BaseSpeed *= CrouchSpeedMultiplier;
    }

    float DirectionMultiplier = 1.0f;

    if (bBackward)
    {
        DirectionMultiplier = BackwardSpeedMultiplier;
    }
    else if (bSide && !bForward)
    {
        DirectionMultiplier = SidewaysSpeedMultiplier;
    }
    else if (bForward && bSide)
    {
        DirectionMultiplier = SidewaysSpeedMultiplier;
    }

    const float SlopeMultiplier = CalculateSlopeSpeedMultiplier();

    return BaseSpeed * DirectionMultiplier * SlopeMultiplier;
}

void AEZCharacter::UpdateMovementSettings()
{
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (!MoveComp)
    {
        return;
    }

    if (bWantsToSprint && !CanSprint())
    {
        bWantsToSprint = false;
    }

    const float SlopeMultiplier = CalculateSlopeSpeedMultiplier();

    float NewAcceleration = MovementAcceleration;
    float NewDeceleration = MovementDeceleration;

    if (bUseSlopeAccelerationModifier)
    {
        if (SlopeMultiplier < 1.0f)
        {
            const float UphillAccelFactor = FMath::Lerp(
                MinUphillAccelerationMultiplier,
                1.0f,
                FMath::Clamp((SlopeMultiplier - MinSprintUphillSpeedMultiplier) / FMath::Max(1.0f - MinSprintUphillSpeedMultiplier, 0.01f), 0.0f, 1.0f)
            );

            NewAcceleration *= UphillAccelFactor;
        }
        else if (SlopeMultiplier > 1.0f)
        {
            const float DownhillDecelFactor = FMath::Clamp(
                2.0f - SlopeMultiplier,
                MinDownhillDecelerationMultiplier,
                1.0f
            );

            NewDeceleration *= DownhillDecelFactor;
        }
    }

    MoveComp->MaxAcceleration = NewAcceleration;
    MoveComp->BrakingDecelerationWalking = NewDeceleration;
    MoveComp->GroundFriction = GroundFriction;
    MoveComp->MaxWalkSpeed = CalculateCurrentMaxSpeed();
}

void AEZCharacter::UpdateView(float DeltaTime)
{
    if (!ViewRootComponent)
    {
        return;
    }

    FVector CurrentLocation = ViewRootComponent->GetRelativeLocation();
    FRotator CurrentRotation = ViewRootComponent->GetRelativeRotation();

    const FVector TargetLocation(0.0f, TargetLeanOffsetY, TargetViewZ);
    const FRotator TargetRotation(0.0f, FreeLookYaw, TargetLeanRoll);

    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, ViewHeightInterpSpeed);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, LeanInterpSpeed);

    if (bIsFreeLooking)
    {
        NewRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, FreeLookPitch, DeltaTime, LeanInterpSpeed);
    }
    else
    {
        NewRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
    }

    ViewRootComponent->SetRelativeLocation(NewLocation);
    ViewRootComponent->SetRelativeRotation(NewRotation);
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