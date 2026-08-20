#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UEZCharacterMovementComponent.generated.h"

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

    void SetSprintBlockedByFreeLook(bool bBlocked);
    void SetSprintBlockedByLean(bool bBlocked);
    void SetSprintBlockedByExternalState(bool bBlocked);

    void IncreaseWalkSpeedStep();
    void DecreaseWalkSpeedStep();

    bool CanSprint() const;
    float GetCurrentGroundAngleDegrees() const;
    float CalculateSlopeSpeedMultiplier() const;
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
    // 0 = disabled

protected:
    UPROPERTY(Transient)
    FVector2D CurrentMoveInput = FVector2D::ZeroVector;

    UPROPERTY(Transient)
    bool bWantsToSprint = false;

    UPROPERTY(Transient)
    bool bSprintBlockedByFreeLook = false;

    UPROPERTY(Transient)
    bool bSprintBlockedByLean = false;

    UPROPERTY(Transient)
    bool bSprintBlockedByExternalState = false;

    UPROPERTY(Transient)
    int32 CurrentWalkSpeedStepIndex = 4;

protected:
    float GetCurrentWalkStepMultiplier() const;
    float CalculateDirectionalSpeedMultiplier() const;
    bool IsTryingToMoveForwardOnly() const;
    bool IsOwnerCrouched() const;
    bool IsSprintBlocked() const;
};