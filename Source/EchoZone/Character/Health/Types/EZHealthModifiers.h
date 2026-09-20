#pragma once

#include "CoreMinimal.h"
#include "EZHealthModifiers.generated.h"

USTRUCT(BlueprintType)
struct FEZHealthModifiers
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float MovementSpeedMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float MaxStaminaMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float StaminaDrainMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float StaminaRecoveryMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float WeaponSwayMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float HipFireSpreadMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float RecoilMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float ReloadTimeMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float ActionDurationMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float CarryWeightMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float CarryWeightFlatBonus = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	float LookSensitivityMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanSprint = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanAim = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanInteract = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanEquipTwoHandedWeapon = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bPistolOnly = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bStanceChangeLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bHideHUD = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bDeafened = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bDoubleVision = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bFreeSprint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bBlockStaminaRecovery = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanConsumeDryFood = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCanConsumePills = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bPoisonBlurActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bCameraDisorientation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Modifiers")
	bool bHallucinationsActive = false;
};