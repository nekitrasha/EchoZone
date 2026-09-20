#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.generated.h"

UENUM(BlueprintType)
enum class EEZLifeState : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	Incapacitated UMETA(DisplayName = "Incapacitated"),
	Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EEZBodyPart : uint8
{
	Head UMETA(DisplayName = "Head"),
	Torso UMETA(DisplayName = "Torso"),
	LeftArm UMETA(DisplayName = "Left Arm"),
	RightArm UMETA(DisplayName = "Right Arm"),
	LeftLeg UMETA(DisplayName = "Left Leg"),
	RightLeg UMETA(DisplayName = "Right Leg")
};

UENUM(BlueprintType)
enum class EEZBleedSeverity : uint8
{
	None UMETA(DisplayName = "None"),
	Light UMETA(DisplayName = "Light"),
	Heavy UMETA(DisplayName = "Heavy")
};

UENUM(BlueprintType)
enum class EEZNeedType : uint8
{
	Hydration UMETA(DisplayName = "Hydration"),
	Hunger UMETA(DisplayName = "Hunger"),
	Temperature UMETA(DisplayName = "Temperature")
};

UENUM(BlueprintType)
enum class EEZStatusEffectType : uint8
{
	None UMETA(DisplayName = "None"),
	Poison UMETA(DisplayName = "Poison"),
	Burning UMETA(DisplayName = "Burning"),
	PainShock UMETA(DisplayName = "Pain Shock"),
	Concussion UMETA(DisplayName = "Concussion"),
	ElectricShock UMETA(DisplayName = "Electric Shock"),
	Adrenaline UMETA(DisplayName = "Adrenaline")
};

UENUM(BlueprintType)
enum class EEZContaminationType : uint8
{
	Biological UMETA(DisplayName = "Biological"),
	Radiation UMETA(DisplayName = "Radiation")
};

UENUM(BlueprintType)
enum class EEZContaminationStage : uint8
{
	None UMETA(DisplayName = "None"),
	Stage1 UMETA(DisplayName = "Stage 1"),
	Stage2 UMETA(DisplayName = "Stage 2"),
	Stage3 UMETA(DisplayName = "Stage 3")
};

UENUM(BlueprintType)
enum class EEZBodyStance : uint8
{
	Standing UMETA(DisplayName = "Standing"),
	Crouching UMETA(DisplayName = "Crouching"),
	Prone UMETA(DisplayName = "Prone")
};