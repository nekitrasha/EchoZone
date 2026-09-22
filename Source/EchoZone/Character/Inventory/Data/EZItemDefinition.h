#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EchoZone/Inventory/Types/EZItemTypes.h"
#include "EchoZone/Inventory/Types/EZAttachmentItemType.h"
#include "EchoZone/Inventory/Types/EZAttachmentSlotType.h"
#include "EchoZone/Inventory/Types/EZPouchType.h"
#include "EchoZone/Inventory/Types/EZStorageRuleType.h"
#include "EchoZone/Inventory/Types/EZWearableConfig.h"
#include "EZItemDefinition.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class ECHOZONE_API UEZItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;
};
