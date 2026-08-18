#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AEZPlayerController.generated.h"

UCLASS()
class ECHOZONE_API AEZPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AEZPlayerController();

protected:
    virtual void BeginPlay() override;
};
