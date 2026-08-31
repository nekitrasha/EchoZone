#pragma once

#include "CoreMinimal.h"
#include "EEZWeaponFeedType.generated.h"


UENUM(BlueprintType)
enum class EEZWeaponFeedType : uint8
{
	None UMETA(DisplayName = "None"),
	DetachableMagazine UMETA(DisplayName = "Detachable Magazine"),
	InternalMagazine UMETA(DisplayName = "Internal Magazine"),
	Tubular UMETA(DisplayName = "Tubular"),
	Cylinder UMETA(DisplayName = "Cylinder"),
	SingleShot UMETA(DisplayName = "Single Shot"),
	BeltFed UMETA(DisplayName = "Belt Fed"),
	EnergyCell UMETA(DisplayName = "Energy Cell")
};