#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "EZStatusEffectState.generated.h"

USTRUCT(BlueprintType)
struct FEZStatusEffectState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	EEZStatusEffectType EffectType = EEZStatusEffectType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float Duration = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float TimeRemaining = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	EEZBodyPart SourceBodyPart = EEZBodyPart::Torso;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	FName InstanceId = NAME_None;
};