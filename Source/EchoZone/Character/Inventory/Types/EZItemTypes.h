#pragma once

#include "CoreMinimal.h"
#include "EZItemTypes.generated.h"

UENUM(BlueprintType)
enum class EEZItemType : uint8
{
	None,

	Ammo,
	Magazine,
	Weapon,

	Medical,
	Food,
	Drink,

	Clothing,
	Armor,
	Helmet,
	GasMask,
	Glasses,
	EarPro,

	Backpack,
	Rig,
	Belt,

	Artifact,
	ArtifactContainer,
	ProtectiveContainer,

	Attachment,
	Pouch,

	Misc
};