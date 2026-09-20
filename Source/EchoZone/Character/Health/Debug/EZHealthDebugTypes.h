#pragma once

#include "CoreMinimal.h"
#include "EZHealthDebugTypes.generated.h"

USTRUCT(BlueprintType)
struct FEZHealthDebugFlags
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrintDamageEvents = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrintStatusEvents = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrintNeedEvents = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawOnScreenSnapshot = false;
};