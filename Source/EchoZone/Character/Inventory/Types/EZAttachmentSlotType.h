#pragma once

#include "CoreMinimal.h"
#include "EZAttachmentSlotType.generated.h"

UENUM(BlueprintType)
enum class EEZAttachmentSlotType : uint8
{
	None,

	HelmetFront,
	HelmetSideLeft,
	HelmetSideRight,

	WeaponSling,

	PlateFront,
	PlateBack,
	PlateSideLeft,
	PlateSideRight,

	ArmorNeck,
	ArmorShoulders,
	ArmorGroin,
	ArmorThighLeft,
	ArmorThighRight,

	Molle1,
	Molle2,
	Molle3,
	Molle4,
	Molle5,
	Molle6,
	Molle7,
	Molle8,
	Molle9,
	Molle10,
	Molle11,
	Molle12,

	FixedStol1,
	FixedStol2,
	FixedStol3,
	FixedStol4,
	FixedStol5,
	FixedStol6,
	FixedStol7,
	FixedStol8,
	FixedStol9,

	HolsterArmor,
	HolsterBelt,
	HolsterThigh,

	BackpackMount1,
	BackpackMount2,
	BackpackMount3,
	BackpackMount4,

	BackWeaponCarry
};
