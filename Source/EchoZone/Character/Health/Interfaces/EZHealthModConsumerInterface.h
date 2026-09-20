#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EchoZone/Character/Health/Types/EZHealthModifiers.h"
#include "EZHealthModConsumerInterface.generated.h"

UINTERFACE(BlueprintType)
class ECHOZONE_API UEZHealthModConsumerInterface : public UInterface
{
	GENERATED_BODY()
};

class ECHOZONE_API IEZHealthModConsumerInterface
{
	GENERATED_BODY()

public:
	virtual void OnHealthModifiersUpdated(const FEZHealthModifiers& NewModifiers) = 0;
};