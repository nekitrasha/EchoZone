#pragma once

#include "CoreMinimal.h"
#include "EchoZone/Weapon/DataAsset/UEZMagazineDataAsset.h"
#include "FEZMagazineInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FEZMagazineInstance 
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine")
	TObjectPtr<UEZMagazineDataAsset> MagazineData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine")
	int32 CurrentAmmo = 0;

	bool IsValid() const
	{
		return MagazineData != nullptr;
	}

	int32 GetCapacity() const
	{
		return MagazineData ? MagazineData->Capacity : 0;
	}

	bool IsEmpty() const
	{
		return CurrentAmmo <= 0;
	}
};
