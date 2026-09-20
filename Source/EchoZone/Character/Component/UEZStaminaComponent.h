#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEZStaminaComponent.generated.h"

class UEZCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaRecovered);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOZONE_API UEZStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEZStaminaComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaDepleted OnStaminaDepleted;

	UPROPERTY(BlueprintAssignable, Category = "Stamina")
	FOnStaminaRecovered OnStaminaRecovered;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float SprintDrainPerSecond = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float RecoveryPerSecond = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float RecoveryDelayAfterSprint = 1.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float MinStaminaToAllowSprint = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	bool bEnableStaminaSystem = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Debug")
	bool bEnableStaminaDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope")
	bool bUseSlopeStaminaModifier = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope", meta = (ClampMin = "0.0"))
	float UphillDrainMultiplierBonus = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope", meta = (ClampMin = "0.0"))
	float DownhillDrainReduction = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope", meta = (ClampMin = "0.0"))
	float UphillRecoveryReduction = 0.50f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope", meta = (ClampMin = "0.0"))
	float DownhillRecoveryBonus = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina|Slope", meta = (ClampMin = "1.0", ClampMax = "89.0"))
	float MaxSlopeAngleForStaminaEffect = 45.0f;

protected:
	UPROPERTY(Transient)
	float TimeSinceSprintStopped = 0.0f;

	UPROPERTY(Transient)
	bool bWasSprintActiveLastTick = false;

	UPROPERTY(Transient)
	bool bStaminaBlockedSprint = false;

	UPROPERTY(Transient)
	UEZCharacterMovementComponent* CachedMovementComponent = nullptr;

protected:
	UPROPERTY(Transient)
	float ExternalMaxStaminaMultiplier = 1.0f;

	UPROPERTY(Transient)
	float ExternalDrainMultiplier = 1.0f;

	UPROPERTY(Transient)
	float ExternalRecoveryMultiplier = 1.0f;

	UPROPERTY(Transient)
	bool bExternalRecoveryBlocked = false;

	UPROPERTY(Transient)
	bool bFreeSprint = false;

	UPROPERTY(Transient)
	bool bManualSprintState = false;

protected:
	void UpdateStamina(float DeltaTime);
	void ApplySprintBlock(bool bBlocked);
	bool CanRecoverStamina() const;
	bool IsSprintCurrentlyActive() const;
	void BroadcastStaminaChanged();
	void DrawDebug() const;

	float GetSlopeStaminaMultiplier() const;
	float GetSlopeRecoveryMultiplier() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetMaxStamina() const { return GetEffectiveMaxStamina(); }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetStaminaNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool IsStaminaEmpty() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool IsSprintBlockedByStamina() const { return bStaminaBlockedSprint; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void AddStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetCurrentStamina(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetSprinting(bool bNewSprinting);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	void SetExternalMaxStaminaMultiplier(float Value);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	void SetExternalDrainMultiplier(float Value);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	void SetExternalRecoveryMultiplier(float Value);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	void SetStaminaRecoveryBlocked(bool bBlocked);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	void SetFreeSprint(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Stamina|External")
	float GetEffectiveMaxStamina() const;
};