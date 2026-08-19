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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:

	//Компоненты

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ViewRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	//Инпуты

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
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

	//Модернизация Movement

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

	//Регулировка скорости ходьбы

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<float> WalkSpeedSteps = { 0.35f, 0.5f, 0.7f, 0.85f, 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	int32 DefaultWalkSpeedStepIndex = 4;

	//НАклоны

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanAngle = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanOffset = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanInterpSpeed = 10.0f;

	//Камера в присяде

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float StandingViewZ = 64.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CrouchedViewZ = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float ViewHeightInterpSpeed = 10.0f;

	//Свободное двжиение камеры

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

	void ToggleCrouch();

	void StartLeanLeft();
	void StopLeanLeft();

	void StartLeanRight();
	void StopLeanRight();

	void StartFreeLook();
	void StopFreeLook();

	void IncreaseWalkSpeedStep();
	void DecreaseWalkSpeedStep();

	void UpdateMovementSettings();
	float CalculateCurrentMaxSpeed() const;
	float GetCurrentWalkStepMultiplier() const;

	bool CanSprint();
	bool IsTryingToMoveForwardOnly();

	void UpdateView(float DeltaTime);
	void UpdateFreeLook(float DeltaTime);
	void CancelSprint();
};
