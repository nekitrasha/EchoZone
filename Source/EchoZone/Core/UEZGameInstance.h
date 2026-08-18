#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UEZGameInstance.generated.h"

UCLASS()
class ECHOZONE_API UEZGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
};
