#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "EZContaminationState.generated.h"

USTRUCT(BlueprintType)
struct FEZContaminationState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Contamination")
	float Value = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Contamination")
	EEZContaminationStage Stage = EEZContaminationStage::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Contamination")
	float LethalTimerRemaining = -1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Contamination")
	float ProcAccumulator = 0.0f;
};