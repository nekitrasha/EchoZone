#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UEZCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ELocomotionStance : uint8
{
    Standing UMETA(DisplayName = "Standing"),
    Crouched UMETA(DisplayName = "Crouched")
};

UENUM(BlueprintType, meta = (Bitflags))
enum class EMovementBlockFlags : uint8
{
    None = 0         UMETA(Hidden),
    FreeLook = 1 << 0,
    Lean = 1 << 1,
    Vault = 1 << 2,
    Stamina = 1 << 3,
    Aiming = 1 << 4,
    External = 1 << 5
};

ENUM_CLASS_FLAGS(EMovementBlockFlags)

UCLASS()
class ECHOZONE_API UEZCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UEZCharacterMovementComponent();

    virtual void BeginPlay() override;
    virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
    virtual float GetMaxSpeed() const override;
    virtual float GetMaxAcceleration() const override;

public:
    void SetMoveInput(const FVector2D& NewMoveInput);

    void SetSprintIntent(bool bEnabled);
    bool IsSprintRequested() const { return bWantsToSprint; }
    bool IsSprintActive() const;

    void IncreaseWalkSpeedStep();
    void DecreaseWalkSpeedStep();

    void SetLocomotionStance(ELocomotionStance NewStance);
    ELocomotionStance GetLocomotionStance() const { return CurrentStance; }

    void AddMovementBlockFlag(EMovementBlockFlags Flag);
    void RemoveMovementBlockFlag(EMovementBlockFlags Flag);
    void SetMovementBlockFlag(EMovementBlockFlags Flag, bool bEnabled);
    bool HasMovementBlockFlag(EMovementBlockFlags Flag) const;

    bool CanSprint() const;
    float GetCurrentGroundAngleDegrees() const;
    float CalculateSlopeSpeedMultiplier() const;
    float GetCurrentWalkStepMultiplier() const;
    float CalculateDirectionalSpeedMultiplier() const;

    void RefreshMovementSettings();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Speed")
    float ForwardWalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Speed")
    float SprintForwardSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Speed")
    float SidewaysSpeedMultiplier = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Speed")
    float BackwardSpeedMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Speed")
    float CrouchSpeedMultiplier = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Acceleration")
    float BaseMovementAcceleration = 850.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Acceleration")
    float BaseMovementDeceleration = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Acceleration")
    float BaseGroundFriction = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Walk Speed Regulator")
    TArray<float> WalkSpeedSteps = { 0.35f, 0.5f, 0.7f, 0.85f, 1.0f };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Walk Speed Regulator")
    int32 DefaultWalkSpeedStepIndex = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    bool bUseSlopeSpeedModifier = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float SlopeEffectDeadZoneAngle = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MaxUphillAngleWalk = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MaxUphillAngleSprint = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MaxDownhillAngle = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MinWalkUphillSpeedMultiplier = 0.72f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MinSprintUphillSpeedMultiplier = 0.60f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MaxDownhillSpeedMultiplier = 1.04f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float CrossSlopeInfluence = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    bool bUseSlopeAccelerationModifier = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MinUphillAccelerationMultiplier = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Slope")
    float MinDownhillDecelerationMultiplier = 0.90f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Sprint")
    float SprintMaxAllowedSlopeAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tactical Movement|Debug")
    bool bEnableMovementDebug = false;

protected:
    UPROPERTY(Transient)
    FVector2D CurrentMoveInput = FVector2D::ZeroVector;

    UPROPERTY(Transient)
    bool bWantsToSprint = false;

    UPROPERTY(Transient)
    int32 CurrentWalkSpeedStepIndex = 4;

    UPROPERTY(Transient)
    ELocomotionStance CurrentStance = ELocomotionStance::Standing;

    UPROPERTY(Transient)
    EMovementBlockFlags MovementBlockFlags = EMovementBlockFlags::None;

protected:
    bool IsTryingToMoveForwardOnly() const;
    bool IsSprintBlocked() const;
    bool IsOwnerActuallyCrouched() const;
    FString GetMovementBlockFlagsString() const;
    FString GetStanceString() const;
    void DrawMovementDebug() const;
};