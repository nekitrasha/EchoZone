// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEZStaminaComponent.generated.h"

class UEZCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaRecovered);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOZONE_API UEZStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEZStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
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
	void UpdateStamina(float DeltaTime);
	void ApplySprintBlock(bool bBlocked);
	bool CanRecoverStamina() const;
	bool IsSprintCurrentlyActive() const;
	void BroadcastStaminaChanged();
	void DrawDebug() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetMaxStamina() const { return MaxStamina; }

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
};
