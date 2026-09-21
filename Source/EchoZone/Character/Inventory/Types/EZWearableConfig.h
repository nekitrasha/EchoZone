#pragma once

#include "CoreMinimal.h"
#include "EZCharacterEquipSlot.h"
#include "EZAttachmentSlotType.h"
#include "EZStorageRuleType.h"
#include "EZPlatformType.h"
#include "EZWearableConfig.generated.h"

USTRUCT(BlueprintType)
struct FEZWearableAttachmentSlotConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	EEZAttachmentSlotType SlotType = EEZAttachmentSlotType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bEnabledByDefault = true;
};

USTRUCT(BLueprintType)
struct FEZWearableStorageSlotConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<EEZCharacterEquipSlot> AllowedCharacterSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	EEZPlatformType PlatformType = EEZPlatformType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<FEZWearableAttachmentSlotConfig> AttachmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<FEZWearableStorageSlotConfig> InternalStorageSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bSupportsRigOverArmor = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bIsMollePlatform = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bProvidesSidearmCarry = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bProvidesExtraLongGunCarry = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	int32 ArtifactSlotProvided = 0;
};