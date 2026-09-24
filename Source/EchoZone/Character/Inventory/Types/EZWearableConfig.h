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

USTRUCT(BlueprintType)
struct FEZWearableStorageSlotConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	EEZStorageRuleType Rule = EEZStorageRuleType::AnyItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bEnabledByDefault = true;
};


USTRUCT(BlueprintType)
struct FEZWearableConfig
{
	GENERATED_BODY()

	// Куда надевается на персонажа
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<EEZCharacterEquipSlot> AllowedCharacterSlots;

	// Что это за платформа
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	EEZPlatformType PlatformType = EEZPlatformType::None;

	// Слоты креплений на этом предмете
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<FEZWearableAttachmentSlotConfig> AttachmentSlots;

	// Встроенные storage-ячейки
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	TArray<FEZWearableStorageSlotConfig> InternalStorageSlots;

	// Логика совместимости
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bSupportsRigOverArmor = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bIsMollePlatform = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bProvidesSidearmCarry = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bProvidesExtraLongGunCarry = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	int32 ArtifactSlotsProvided = 0;
};