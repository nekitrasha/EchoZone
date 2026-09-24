#include "EchoZone/Character/Inventory/Data/EZItemDefinition.h"
#include "EchoZone/Character/Inventory/Types/EZAttachmentSlotType.h"
#include "EZItemInstanceObject.h"

bool UEZItemInstanceObject::IsStackable() const
{
	return Definition && Definition->bStackable;
}

bool UEZItemInstanceObject::IsWearable() const
{
	return Definition && Definition->bIsWearable;
}

bool UEZItemInstanceObject::IsAttachment() const
{
	return Definition && Definition->bIsAttachment;
}

bool UEZItemInstanceObject::IsPouch() const
{
	return Definition && Definition->bIsPouch;
}

bool UEZItemInstanceObject::HasWeaponSling() const
{
	for (const FEZRuntimeAttachmentSlot& Slot : RuntimeAttachmentSlots)
	{
		if (Slot.SlotType == EEZAttachmentSlotType::WeaponSling && Slot.AttachedItem != nullptr)
		{
			return true;
		}
	}

	return false;
}

void UEZItemInstanceObject::InitializeFromDefinition(UEZItemDefinition* InDefinition)
{
	Definition = InDefinition;
	RuntimeAttachmentSlots.Empty();
	RuntimeStorageSlots.Empty();

	if (!Definition)
	{
		return;
	}

	if (Definition->bStackable)
	{
		Quantity = 1;
	}
	else
	{
		Quantity = 1;
	}

	if (Definition->bIsWearable)
	{
		for (const FEZWearableAttachmentSlotConfig& AttachmentConfig : Definition->WearableConfig.AttachmentSlots)
		{
			FEZRuntimeAttachmentSlot NewSlot;
			NewSlot.SlotType = AttachmentConfig.SlotType;
			NewSlot.bEnabled = AttachmentConfig.bEnabledByDefault;
			NewSlot.AttachedItem = nullptr;
			RuntimeAttachmentSlots.Add(NewSlot);
		}

		for (const FEZWearableStorageSlotConfig& StorageConfig : Definition->WearableConfig.InternalStorageSlots)
		{
			FEZRuntimeStorageSlot NewSlot;
			NewSlot.Rule = StorageConfig.Rule;
			NewSlot.bEnabled = StorageConfig.bEnabledByDefault;
			NewSlot.StoredItem = nullptr;
			RuntimeStorageSlots.Add(NewSlot);
		}
	}

	if (Definition->bIsPouch)
	{
		for (int32 i = 0; i < Definition->PouchCapacity; ++i)
		{
			FEZRuntimeStorageSlot NewSlot;
			NewSlot.Rule = Definition->PouchRule;
			NewSlot.bEnabled = true;
			NewSlot.StoredItem = nullptr;
			RuntimeStorageSlots.Add(NewSlot);
		}
	}
}