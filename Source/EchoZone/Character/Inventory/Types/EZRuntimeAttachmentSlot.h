#pragma once

#include "CoreMinimal.h"
#include "EZAttachmentSlotType.h"
#include "EZRuntimeAttachmentSlot.generated.h"

class UEZItemInstanceObject;

USTRUCT(BlueprintType)
struct FEZRuntimeAttachmentSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	EEZAttachmentSlotType SlotType = EEZAttachmentSlotType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment")
	TObjectPtr<UEZItemInstanceObject> AttachedItem = nullptr;

	bool IsEmpty() const
	{
		return AttachedItem == nullptr;
	}
};
