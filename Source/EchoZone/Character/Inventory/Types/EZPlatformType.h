#pragma once

#include "CoreMinimal.h"
#include "EZPlatformType.generated.h"

UENUM(BlueprintType)
enum class EEZPlatformType : uint8
{
	None,
	Clothing,
	HeimetPlatform,
	ArmorPlatform,
	RigPlatform,
	BeltPlatform,
	BackpackPlatform,
	ContainerPlatform,
	WeaponPlatform
};
