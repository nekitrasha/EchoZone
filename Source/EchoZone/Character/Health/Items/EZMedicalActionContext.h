#pragma once

#include "CoreMinimal.h"
#include "EchoZone/Character/Health/Types/EZHealthTypes.h"
#include "EZMedicalActionTypes.h"
#include "EZMedicalActionContext.generated.h"

USTRUCT(BlueprintType)
struct FEZMedicalActionContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medical")
	EEZMedicalActionType ActionType = EEZMedicalActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medical")
	EEZBodyPart TargetBodyPart = EEZBodyPart::Torso;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medical")
	float Potency = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medical")
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medical")
	TObjectPtr<AActor> UserActor = nullptr;
};