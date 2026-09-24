#pragma once

#include "CoreMinimal.h"
#include "EZStorageRuleType.h"
#include "EZRuntimeStorageSlot.generated.h"

class UEZItemInstanceObject;

USTRUCT(BlueprintType)
struct FEZRuntimeStorageSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	EEZStorageRuleType Rule = EEZStorageRuleType::AnyItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage")
	TObjectPtr<UEZItemInstanceObject> StoredItem = nullptr;

	bool IsEmpty() const
	{
		return StoredItem == nullptr;
	}
};
