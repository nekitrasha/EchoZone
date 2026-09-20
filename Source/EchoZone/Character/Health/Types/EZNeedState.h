#pragma once

#include "CoreMinimal.h"
#include "EZNeedState.generated.h"

USTRUCT(BlueprintType)
struct FEZNeedState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Need")
	float MaxValue = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Need")
	float CurrentValue = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Need")
	float DecayPerSecond = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Need")
	bool bDepleted = false;

	float GetPercent() const
	{
		return MaxValue > 0.0f ? CurrentValue / MaxValue : 0.0f;
	}
};