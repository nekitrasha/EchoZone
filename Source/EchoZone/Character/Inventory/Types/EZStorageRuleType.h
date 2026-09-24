#pragma once

#include "CoreMinimal.h"
#include "EZStorageRuleType.generated.h"

UENUM(BlueprintType)
enum class EEZStorageRuleType : uint8
{
	None,

	AnyItem,
	MedicalOnly,
	FoodOnly,
	WaterOnly,
	UtilityOnly,
	MagazineOnly,
	AmmoOnly,
	PistolOnly,
	LongGunOnly,
	ArmorPlateOnly,
	ArtifactOnly
};
