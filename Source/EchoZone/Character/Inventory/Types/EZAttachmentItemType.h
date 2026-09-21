#pragma once

#include "CoreMinimal.h"
#include "EZAttachmentItemType.generated.h"

UENUM(BlueprintType)
enum class EEZAttachmentItemType : uint8
{
	None,

	Flashlight,
	NVG,
	ArmorPlate,
	ArmorAddon,
	Pouch,
	Holster,
	WeaponSling,
	UtilityModule
};
