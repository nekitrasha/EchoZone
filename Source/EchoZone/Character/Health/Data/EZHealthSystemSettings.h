#pragma once

#include "CoreMinimal.h"
#include "EZHealthSystemSettings.generated.h"

USTRUCT(BlueprintType)
struct FEZHealthSystemSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BodyParts")
	float CriticalPartMaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BodyParts")
	float LimbMaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BodyParts")
	float BlackLimbOverflowMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Adrenaline")
	float AdrenalineTriggerDamage = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Adrenaline")
	float AdrenalineDuration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bleeding")
	float LightBleedDamagePerSecond = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bleeding")
	float HeavyBleedDamagePerSecond = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bleeding")
	float LightBleedAutoClotTime = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HydrationDecayPerSecond = 0.03f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HungerDecayPerSecond = 0.01f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float DehydrationTorsoDamagePerSecond = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float StarvationTorsoDamagePerSecond = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float DehydrationStaminaMultiplier = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HungerCarryWeightPenalty = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HungerWeaponSwayMultiplier = 1.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HypothermiaThreshold = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HyperthermiaThreshold = 42.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float NormalBodyTemperature = 36.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HypothermiaDamagePerMinute = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HyperthermiaDamagePerMinute = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HypothermiaMoveSpeedMultiplier = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HypothermiaActionDurationMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Needs")
	float HyperthermiaHydrationDecayMultiplier = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float PoisonDamagePerSecond = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float BurnDamagePerSecond = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float BurnDamageMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float PainShockDuration = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ConcussionMinDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ConcussionMaxDuration = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ElectricShockDuration = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ArmFractureWeaponSwayMultiplier = 1.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ArmFractureReloadMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float LegFractureMoveSpeedMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float PainShockMaxStaminaMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float PainShockHipFireSpreadMultiplier = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ElectricShockMoveSpeedMultiplier = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status")
	float ElectricShockWeaponSwayMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalStage1Max = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalStage2Max = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalLethalDuration = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationStage1Max = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationStage2Max = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationLethalDuration = 900.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationStage1DamagePerMinute = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationStage2DamagePerMinute = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float RadiationStage2BleedChancePerMinute = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalStage1PhysicalMultiplier = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalStage2PhysicalMultiplier = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Contamination")
	float BiologicalStage3PhysicalMultiplier = 0.5f;
};