#pragma once

#include "CoreMinimal.h"
#include "EZMedicalActionTypes.generated.h"

UENUM(BlueprintType)
enum class EEZMedicalActionType : uint8
{
	None UMETA(DisplayName = "None"),
	Bandage UMETA(DisplayName = "Bandage"),
	Tourniquet UMETA(DisplayName = "Tourniquet"),
	Splint UMETA(DisplayName = "Splint"),
	Morphine UMETA(DisplayName = "Morphine"),
	Antidote UMETA(DisplayName = "Antidote"),
	Antirad UMETA(DisplayName = "Antirad"),
	BurnTreatment UMETA(DisplayName = "Burn Treatment"),
	Water UMETA(DisplayName = "Water"),
	Food UMETA(DisplayName = "Food"),
	StimPack UMETA(DisplayName = "Stim Pack")
};

UENUM(BlueprintType)
enum class EEZMedicalItemUseResult : uint8
{
	Success UMETA(DisplayName = "Success"),
	Failed_InvalidTarget UMETA(DisplayName = "Invalid Target"),
	Failed_NoEffect UMETA(DisplayName = "No Effect"),
	Failed_Blocked UMETA(DisplayName = "Blocked"),
	Failed_InvalidState UMETA(DisplayName = "Invalid State")
};