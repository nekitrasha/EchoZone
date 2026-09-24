#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EchoZone/Character/Inventory/Types/EZRuntimeAttachmentSlot.h"
#include "EchoZone/Character/Inventory/Types/EZRuntimeStorageSlot.h"
#include "EZItemInstanceObject.generated.h"

class UEZItemDefinition;

UCLASS(BlueprintType)
class ECHOZONE_API UEZItemInstanceObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UEZItemDefinition> Definition = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 Quantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	float Durability = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 LoadedAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	bool bHasChamberedRound = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Medical")
	int32 UsesRemaining = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsContaminated = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime")
	TArray<FEZRuntimeAttachmentSlot> RuntimeAttachmentSlots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Runtime")
	TArray<FEZRuntimeStorageSlot> RuntimeStorageSlots;

public:
	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsValidItem() const
	{
		return Definition != nullptr && Quantity > 0;
	}

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsStackable() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsWearable() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsAttachment() const;

	UFUNCTION(BlueprintPure, Category = "Item")
	bool IsPouch() const;

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool HasWeaponSling() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void InitializeFromDefinition(UEZItemDefinition* InDefinition);
};
