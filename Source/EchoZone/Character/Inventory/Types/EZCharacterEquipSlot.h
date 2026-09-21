#pragma once

#include "CoreMinimal.h"
#include "EZCharacterEquipSlot.generated.h"

UENUM(BlueprintType)
enum class EEZCharacterEquipSlot : uint8
{
	None,

	HeadHelmet,
	HeadMask,
	HeadEyes,
	HeadEars,

	TorsoBase,
	TorsoMid,
	TorsoOuter,
	TorsoSuit,
	TorsoArmor,
	TorsoRig,

	LegsBase,
	LegsOuter,
	LegsArmor,

	FeetBase,
	FeetOuter,

	Hands,

	TacticalBelt,
	BackPack,

	ArtifactContainer,
	ProtectiveContainer,

	PrimaryLongGun,
	SideArm
	
};
