#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EchoZone/Character/Inventory/Types/EZItemTypes.h"
#include "EchoZone/Character/Inventory/Types/EZAttachmentItemType.h"
#include "EchoZone/Character/Inventory/Types/EZAttachmentSlotType.h"
#include "EchoZone/Character/Inventory/Types/EZPouchType.h"
#include "EchoZone/Character/Inventory/Types/EZStorageRuleType.h"
#include "EchoZone/Character/Inventory/Types/EZWearableConfig.h"
#include "EZItemDefinition.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class ECHOZONE_API UEZItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	//Base
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	EEZItemType ItemType = EEZItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Weight = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool bStackable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (EditCondition = "bStacable", ClampMin = "1"))
	int32 MaxStack = 1;

	//Wearable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	bool bIsWearable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wearable")
	FEZWearableConfig WearableConfig;

	//Attachment
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	bool bIsAttachment = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "bIsAttachment"))
	EEZAttachmentItemType AttachmentType = EEZAttachmentItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "bIsAttachment"))
	TArray<EEZAttachmentSlotType> AllowedAttachmentSlot;

	//Pouch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pouch")
	bool bIsPouch = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pouch", meta = (EditCondition = "bIsPouch"))
	EEZPouchType PouchType = EEZPouchType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pouch", meta = (EditCondition = "bIsPouch", ClampMin = "1"))
	int32 PouchCapacity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pouch", meta = (EditCondition = "bIsPouch"))
	EEZStorageRuleType PouchRule = EEZStorageRuleType::AnyItem;

	//Inventory bonuses
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 BonusInventorySlot = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 BonusQuickSlot = 0;

	//Protection
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float BallisticProtection = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float MeleePtrotection = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float RadiationProtection = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float ChemicalProtection = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float BiologicalProtection = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Protection")
	float ThermalProtection = 0.0f;

	// Movement / stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MoveSpeedMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float StaminaRegenMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float StaminaDrainMultiplier = 1.0f;

	// Sling gameplay
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Sling")
	float SidearmRecoilMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Sling")
	float SidearmAccuracyMultiplier = 1.0f;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Sling")
	float WeaponDropProtection = 1.0f;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Sling")
	float SidearmSwapSpeedMultiplier = 1.0f;
};
