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
class UEZCharacterMovementComponent;
class UEZStaminaComponent;
class UEZInteractComponent;
class UEZHealthComponent;

UCLASS()
class ECHOZONE_API AEZCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEZCharacter(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* ViewRootComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEZStaminaComponent* StaminaComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEZInteractComponent* InteractComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UEZHealthComponent* HealthComponent;

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* Interacted;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanAngle = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanOffset = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
    float LeanInterpSpeed = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float StandingViewZ = 64.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float CrouchedViewZ = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float ViewHeightInterpSpeed = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookYawLimit = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookPitchUpLimit = 60.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookPitchDownLimit = 75.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
    float FreeLookReturnInterpSpeed = 8.0f;

protected:
    UPROPERTY(Transient)
    bool bIsFreeLooking = false;

    UPROPERTY(Transient)
    bool bIsLeaningLeft = false;

    UPROPERTY(Transient)
    bool bIsLeaningRight = false;

    UPROPERTY(Transient)
    float TargetLeanRoll = 0.0f;

    UPROPERTY(Transient)
    float TargetLeanOffsetY = 0.0f;

    UPROPERTY(Transient)
    float TargetViewZ = 64.0f;

    UPROPERTY(Transient)
    float FreeLookYaw = 0.0f;

    UPROPERTY(Transient)
    float FreeLookPitch = 0.0f;

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

    void Interact();

    void UpdateView(float DeltaTime);
    void UpdateFreeLook(float DeltaTime);
    void UpdateLeanState();
    void UpdateMovementFlags();

    UEZCharacterMovementComponent* GetUEZMovementComponent() const;
};