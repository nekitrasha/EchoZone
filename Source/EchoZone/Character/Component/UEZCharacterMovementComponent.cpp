#include "UEZCharacterMovementComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/Character.h"

UEZCharacterMovementComponent::UEZCharacterMovementComponent()
{
    CurrentWalkSpeedStepIndex = DefaultWalkSpeedStepIndex;
}

void UEZCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentWalkSpeedStepIndex = FMath::Clamp(DefaultWalkSpeedStepIndex, 0, WalkSpeedSteps.Num() - 1);
    CurrentStance = IsOwnerActuallyCrouched() ? ELocomotionStance::Crouched : ELocomotionStance::Standing;

    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

    RefreshMovementSettings();

    if (bEnableMovementDebug)
    {
        DrawMovementDebug();
    }
}

void UEZCharacterMovementComponent::SetMoveInput(const FVector2D& NewMoveInput)
{
    CurrentMoveInput = NewMoveInput;
}

void UEZCharacterMovementComponent::SetSprintIntent(bool bEnabled)
{
    bWantsToSprint = bEnabled;

    if (bWantsToSprint && !CanSprint())
    {
        bWantsToSprint = false;
    }

    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::IncreaseWalkSpeedStep()
{
    if (WalkSpeedSteps.Num() == 0)
    {
        return;
    }

    CurrentWalkSpeedStepIndex = FMath::Clamp(CurrentWalkSpeedStepIndex + 1, 0, WalkSpeedSteps.Num() - 1);
    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::DecreaseWalkSpeedStep()
{
    if (WalkSpeedSteps.Num() == 0)
    {
        return;
    }

    CurrentWalkSpeedStepIndex = FMath::Clamp(CurrentWalkSpeedStepIndex - 1, 0, WalkSpeedSteps.Num() - 1);
    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::SetLocomotionStance(ELocomotionStance NewStance)
{
    CurrentStance = NewStance;
    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::AddMovementBlockFlag(EMovementBlockFlags Flag)
{
    MovementBlockFlags |= Flag;

    if (bWantsToSprint && !CanSprint())
    {
        bWantsToSprint = false;
    }

    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::RemoveMovementBlockFlag(EMovementBlockFlags Flag)
{
    MovementBlockFlags &= ~Flag;
    RefreshMovementSettings();
}

void UEZCharacterMovementComponent::SetMovementBlockFlag(EMovementBlockFlags Flag, bool bEnabled)
{
    if (bEnabled)
    {
        AddMovementBlockFlag(Flag);
    }
    else
    {
        RemoveMovementBlockFlag(Flag);
    }
}

bool UEZCharacterMovementComponent::HasMovementBlockFlag(EMovementBlockFlags Flag) const
{
    return EnumHasAnyFlags(MovementBlockFlags, Flag);
}

bool UEZCharacterMovementComponent::IsOwnerActuallyCrouched() const
{
    const ACharacter* CharacterOwnerPtr = CharacterOwner.Get();
    return CharacterOwnerPtr && CharacterOwnerPtr->bIsCrouched;
}

bool UEZCharacterMovementComponent::IsSprintBlocked() const
{
    return EnumHasAnyFlags(
        MovementBlockFlags,
        EMovementBlockFlags::FreeLook |
        EMovementBlockFlags::Lean |
        EMovementBlockFlags::Vault |
        EMovementBlockFlags::Stamina |
        EMovementBlockFlags::Aiming |
        EMovementBlockFlags::External
    );
}

bool UEZCharacterMovementComponent::IsTryingToMoveForwardOnly() const
{
    const bool bForward = CurrentMoveInput.Y > 0.1f;
    const bool bSide = FMath::Abs(CurrentMoveInput.X) > 0.1f;
    const bool bBackward = CurrentMoveInput.Y < -0.1f;

    return bForward && !bSide && !bBackward;
}

bool UEZCharacterMovementComponent::CanSprint() const
{
    if (!CharacterOwner)
    {
        return false;
    }

    if (IsSprintBlocked())
    {
        return false;
    }

    if (IsOwnerActuallyCrouched() || CurrentStance == ELocomotionStance::Crouched)
    {
        return false;
    }

    if (!IsMovingOnGround())
    {
        return false;
    }

    if (!IsTryingToMoveForwardOnly())
    {
        return false;
    }

    if (SprintMaxAllowedSlopeAngle > 0.0f && GetCurrentGroundAngleDegrees() > SprintMaxAllowedSlopeAngle)
    {
        return false;
    }

    return true;
}

bool UEZCharacterMovementComponent::IsSprintActive() const
{
    return bWantsToSprint && CanSprint();
}

float UEZCharacterMovementComponent::GetCurrentGroundAngleDegrees() const
{
    if (!IsMovingOnGround() || !CurrentFloor.bBlockingHit)
    {
        return 0.0f;
    }

    const FVector FloorNormal = CurrentFloor.HitResult.ImpactNormal.GetSafeNormal();
    if (FloorNormal.IsNearlyZero())
    {
        return 0.0f;
    }

    const float DotUp = FVector::DotProduct(FloorNormal, FVector::UpVector);
    return FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotUp, -1.0f, 1.0f)));
}

float UEZCharacterMovementComponent::GetCurrentWalkStepMultiplier() const
{
    if (WalkSpeedSteps.IsValidIndex(CurrentWalkSpeedStepIndex))
    {
        return WalkSpeedSteps[CurrentWalkSpeedStepIndex];
    }

    return 1.0f;
}

float UEZCharacterMovementComponent::CalculateDirectionalSpeedMultiplier() const
{
    const bool bForward = CurrentMoveInput.Y > 0.1f;
    const bool bSide = FMath::Abs(CurrentMoveInput.X) > 0.1f;
    const bool bBackward = CurrentMoveInput.Y < -0.1f;

    if (bBackward)
    {
        return BackwardSpeedMultiplier;
    }

    if (bSide && !bForward)
    {
        return SidewaysSpeedMultiplier;
    }

    if (bForward && bSide)
    {
        return SidewaysSpeedMultiplier;
    }

    return 1.0f;
}

float UEZCharacterMovementComponent::CalculateSlopeSpeedMultiplier() const
{
    if (!bUseSlopeSpeedModifier)
    {
        return 1.0f;
    }

    if (!IsMovingOnGround() || CurrentMoveInput.IsNearlyZero() || !CurrentFloor.bBlockingHit || !CharacterOwner)
    {
        return 1.0f;
    }

    const FVector FloorNormal = CurrentFloor.HitResult.ImpactNormal.GetSafeNormal();
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

    const FVector Forward = CharacterOwner->GetActorForwardVector();
    const FVector Right = CharacterOwner->GetActorRightVector();

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

    const float AlignmentToDownhill = FVector::DotProduct(DesiredMoveDirection, DownhillDirection);
    const float DirectionInfluence = FMath::Lerp(CrossSlopeInfluence, 1.0f, FMath::Abs(AlignmentToDownhill));

    if (AlignmentToDownhill < -0.05f)
    {
        const bool bSprintActive = IsSprintActive();
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

float UEZCharacterMovementComponent::GetMaxSpeed() const
{
    float BaseSpeed = ForwardWalkSpeed * GetCurrentWalkStepMultiplier();

    if (IsSprintActive())
    {
        BaseSpeed = SprintForwardSpeed;
    }

    if (IsOwnerActuallyCrouched() || CurrentStance == ELocomotionStance::Crouched)
    {
        BaseSpeed *= CrouchSpeedMultiplier;
    }

    BaseSpeed *= CalculateDirectionalSpeedMultiplier();
    BaseSpeed *= CalculateSlopeSpeedMultiplier();

    return BaseSpeed;
}

float UEZCharacterMovementComponent::GetMaxAcceleration() const
{
    float Result = BaseMovementAcceleration;

    if (CurrentStance == ELocomotionStance::Crouched || IsOwnerActuallyCrouched())
    {
        Result *= 0.8f;
    }

    if (!bUseSlopeAccelerationModifier)
    {
        return Result;
    }

    const float SlopeMultiplier = CalculateSlopeSpeedMultiplier();

    if (SlopeMultiplier < 1.0f)
    {
        const float UphillAccelFactor = FMath::Lerp(
            MinUphillAccelerationMultiplier,
            1.0f,
            FMath::Clamp(
                (SlopeMultiplier - MinSprintUphillSpeedMultiplier) / FMath::Max(1.0f - MinSprintUphillSpeedMultiplier, 0.01f),
                0.0f,
                1.0f
            )
        );

        Result *= UphillAccelFactor;
    }

    return Result;
}

void UEZCharacterMovementComponent::RefreshMovementSettings()
{
    CurrentStance = IsOwnerActuallyCrouched() ? ELocomotionStance::Crouched : ELocomotionStance::Standing;

    if (bWantsToSprint && !CanSprint())
    {
        bWantsToSprint = false;
    }

    MaxWalkSpeed = GetMaxSpeed();
    MaxAcceleration = GetMaxAcceleration();

    float NewDeceleration = BaseMovementDeceleration;

    if (CurrentStance == ELocomotionStance::Crouched || IsOwnerActuallyCrouched())
    {
        NewDeceleration *= 0.9f;
    }

    if (bUseSlopeAccelerationModifier)
    {
        const float SlopeMultiplier = CalculateSlopeSpeedMultiplier();

        if (SlopeMultiplier > 1.0f)
        {
            const float DownhillDecelFactor = FMath::Clamp(
                2.0f - SlopeMultiplier,
                MinDownhillDecelerationMultiplier,
                1.0f
            );

            NewDeceleration *= DownhillDecelFactor;
        }
    }

    BrakingDecelerationWalking = NewDeceleration;
    GroundFriction = BaseGroundFriction;
}

FString UEZCharacterMovementComponent::GetStanceString() const
{
    switch (CurrentStance)
    {
    case ELocomotionStance::Standing: return TEXT("Standing");
    case ELocomotionStance::Crouched: return TEXT("Crouched");
    default:                          return TEXT("Unknown");
    }
}

FString UEZCharacterMovementComponent::GetMovementBlockFlagsString() const
{
    TArray<FString> Names;

    if (HasMovementBlockFlag(EMovementBlockFlags::FreeLook)) Names.Add(TEXT("FreeLook"));
    if (HasMovementBlockFlag(EMovementBlockFlags::Lean))     Names.Add(TEXT("Lean"));
    if (HasMovementBlockFlag(EMovementBlockFlags::Vault))    Names.Add(TEXT("Vault"));
    if (HasMovementBlockFlag(EMovementBlockFlags::Stamina))  Names.Add(TEXT("Stamina"));
    if (HasMovementBlockFlag(EMovementBlockFlags::Aiming))   Names.Add(TEXT("Aiming"));
    if (HasMovementBlockFlag(EMovementBlockFlags::External)) Names.Add(TEXT("External"));

    if (Names.Num() == 0)
    {
        return TEXT("None");
    }

    FString Result;
    for (int32 Index = 0; Index < Names.Num(); ++Index)
    {
        Result += Names[Index];
        if (Index < Names.Num() - 1)
        {
            Result += TEXT(" | ");
        }
    }

    return Result;
}

void UEZCharacterMovementComponent::DrawMovementDebug() const
{
    if (!GEngine || !CharacterOwner)
    {
        return;
    }

    const FString DebugText = FString::Printf(
        TEXT("Stance: %s\nSprint Requested: %s\nSprint Active: %s\nBlockers: %s\nGround Angle: %.2f\nSlope Mult: %.3f\nMax Speed: %.2f\nCurrent Speed: %.2f\nWalk Step: %d"),
        *GetStanceString(),
        bWantsToSprint ? TEXT("Yes") : TEXT("No"),
        IsSprintActive() ? TEXT("Yes") : TEXT("No"),
        *GetMovementBlockFlagsString(),
        GetCurrentGroundAngleDegrees(),
        CalculateSlopeSpeedMultiplier(),
        GetMaxSpeed(),
        Velocity.Size2D(),
        CurrentWalkSpeedStepIndex
    );

    GEngine->AddOnScreenDebugMessage(
        reinterpret_cast<uint64>(this),
        0.0f,
        FColor::Green,
        DebugText
    );
}