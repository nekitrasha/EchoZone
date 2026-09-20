#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EchoZone/Character/Health/Types/EZHealthEventPayloads.h"
#include "EchoZone/Character/Health/Types/EZHealthTypes.h"
#include "AEZCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
class AEZWeaponBase;
class UEZCharacterMovementComponent;
class UEZStaminaComponent;
class UEZInteractComponent;
class UEZHealthComponent;
class UEZInteractWidget;
class UEZWeaponPresentationComponent;

UCLASS()
class ECHOZONE_API AEZCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEZCharacter(const FObjectInitializer& ObjectInitializer);

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ViewRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* ADSAnchor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEZWeaponPresentationComponent* WeaponPresentationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEZStaminaComponent* StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEZInteractComponent* InteractComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UEZHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEZInteractWidget> InteractWidgetClass;

	UPROPERTY()
	UEZInteractWidget* InteractWidget;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SwitchFireModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanAngle = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanOffset = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean")
	float LeanInterpSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FName FirstPersonCameraSocketName = TEXT("fp_camera_socket");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float StandingViewZ = 64.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CrouchedViewZ = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float ViewHeightInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float DefaultFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float FOVInterpSpeedFallback = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
	float FreeLookYawLimit = 85.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
	float FreeLookPitchUpLimit = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
	float FreeLookPitchDownLimit = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FreeLook")
	float FreeLookReturnInterpSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEZWeaponBase> StarterWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AEZWeaponBase* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName WeaponSocketName = TEXT("weapon_r_socket");

protected:
	UPROPERTY(Transient)
	bool bIsFreeLooking = false;

	UPROPERTY(Transient)
	bool bIsLeaningLeft = false;

	UPROPERTY(Transient)
	bool bIsLeaningRight = false;

	UPROPERTY(Transient)
	bool bIsWeaponAiming = false;

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

	UFUNCTION()
	void HandleInteractableChanged(AActor* NewInteractable);

	void UpdateInteractWidget();

	void UpdateView(float DeltaTime);
	void UpdateFreeLook(float DeltaTime);
	void UpdateLeanState();
	void UpdateMovementFlags();
	void UpdateWeaponFOV(float DeltaTime);
	void SyncWeaponPresentation();

	void StartFire();
	void StopFire();
	void ReloadWeapon();
	void StartAim();
	void StopAim();
	void SwitchFireMode();
	void EquipStarterWeapon();

	UEZCharacterMovementComponent* GetUEZMovementComponent() const;

protected:
	UFUNCTION()
	void HandleHealthDeath();

	UFUNCTION()
	void HandleDropWeaponRequested();

	UFUNCTION()
	void HandleScreamRequested();

	UFUNCTION()
	void HandleHealthModifiersChanged(const FEZModifiersChangedEvent& EventData);

	void SyncHealthState();
	void ApplyHealthModifiersToCharacter();

	EEZBodyStance GetCurrentHealthStance() const;
	bool CanAimByHealth() const;
	bool CanInteractByHealth() const;
};