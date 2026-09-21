#pragma once

#include "CoreMinimal.h"
#include "EZPouchType.generated.h"

UENUM(BlueprintType)
enum class EEZPouchType : uint8
{
	None,
	Medical,
	Food,
	Water,
	Utility,
	Magazine,
	General
};
