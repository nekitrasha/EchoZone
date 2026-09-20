#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "EZBodyPartState.generated.h"

USTRUCT(BlueprintType)
struct FEZBodyPartState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bDestroyed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bFractured = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bBurned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	EEZBleedSeverity BleedSeverity = EEZBleedSeverity::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float BurnDamageMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float AutoClotProgress = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float AccumulatedPain = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float LastDamageGameTime = 0.0f;

	float GetHealthPercent() const
	{
		return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
	}
};