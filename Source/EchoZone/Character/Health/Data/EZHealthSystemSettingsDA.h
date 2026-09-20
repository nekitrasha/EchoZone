#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EZHealthSystemSettings.h"
#include "EZHealthSystemSettingsDA.generated.h"

UCLASS(BlueprintType)
class ECHOZONE_API UEZHealthSystemSettingsDA : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FEZHealthSystemSettings Settings;
};