#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "EZHealthModifiers.h"
#include "EZHealthSnapshot.generated.h"

USTRUCT(BlueprintType)
struct FEZHealthSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	EEZLifeState LifeState = EEZLifeState::Alive;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	FEZHealthModifiers Modifiers;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float HydrationPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float HungerPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float BodyTemperature = 36.6f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float HeadHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float TorsoHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float LeftArmHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float RightArmHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float LeftLegHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	float RightLegHealthPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	EEZContaminationStage BiologicalStage = EEZContaminationStage::None;

	UPROPERTY(BlueprintReadOnly, Category = "Snapshot")
	EEZContaminationStage RadiationStage = EEZContaminationStage::None;
};
