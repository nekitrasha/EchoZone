#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AEZCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ECHOZONE_API AEZCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEZCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* ViewRootComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    // Input
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LeanLeftAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LeanRightAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* FreeLookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* WalkSpeedUpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* WalkSpeedDownAction;

    // Base movement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float ForwardWalkSpeed = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float SprintForwardSpeed = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float SidewaysSpeedMultiplier = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float BackwardSpeedMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float CrouchSpeedMultiplier = 0.55f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MovementAcceleration = 850.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float MovementDeceleration = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float GroundFriction = 6.0f;

    // Walk speed regulator
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    TArray<float> WalkSpeedSteps = { 0.35f, 0.5f, 0.7f, 0.85f, 1.0f };

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    int32 DefaultWalkSpeedStepIndex = 4;

    // Slope movement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    bool bUseSlopeSpeedModifier = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float SlopeEffectDeadZoneAngle = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MaxUphillAngleWalk = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MaxUphillAngleSprint = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MaxDownhillAngle = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MinWalkUphillSpeedMultiplier = 0.72f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MinSprintUphillSpeedMultiplier = 0.60f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MaxDownhillSpeedMultiplier = 1.04f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float CrossSlopeInfluence = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    bool bUseSlopeAccelerationModifier = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MinUphillAccelerationMultiplier = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float MinDownhillDecelerationMultiplier = 0.90f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Slope")
    float SprintMaxAllowedSlopeAngle = 0.0f;
    // 0 = выключено. Если > 0, можно запретить спринт на крутых уклонах.

    // Lean
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanAngle = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanOffset = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanInterpSpeed = 10.0f;

    // Camera / stance
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float StandingViewZ = 64.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float CrouchedViewZ = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float ViewHeightInterpSpeed = 10.0f;

    // Free look
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookYawLimit = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookPitchUpLimit = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookPitchDownLimit = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookReturnInterpSpeed = 8.0f;

protected:
    FVector2D CurrentMoveInput = FVector2D::ZeroVector;

    bool bWantsToSprint = false;
    bool bIsFreeLooking = false;

    float TargetLeanRoll = 0.0f;
    float TargetLeanOffsetY = 0.0f;
    float TargetViewZ = 64.0f;

    float FreeLookYaw = 0.0f;
    float FreeLookPitch = 0.0f;

    int32 CurrentWalkSpeedStepIndex = 4;

protected:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    void StartSprint();
    void StopSprint();

    void StartCrouch();
    void StopCrouch();

    void StartLeanLeft();
    void StopLeanLeft();

    void StartLeanRight();
    void StopLeanRight();

    void StartFreeLook();
    void StopFreeLook();

    void IncreaseWalkSpeedStep();
    void DecreaseWalkSpeedStep();

    void UpdateMovementSettings();
    void UpdateView(float DeltaTime);
    void UpdateFreeLook(float DeltaTime);

    float CalculateCurrentMaxSpeed() const;
    float GetCurrentWalkStepMultiplier() const;
    float CalculateSlopeSpeedMultiplier() const;
    float GetCurrentGroundAngleDegrees() const;

    bool CanSprint() const;
    bool IsTryingToMoveForwardOnly() const;

    void CancelSprint();
};