#pragma once

#include "CoreMinimal.h"
#include "EchoZone/Character/Health/Types/EZHealthTypes.h"
#include "EchoZOne/Character/Health/Data/EZHealthSystemSettings.h"

class ECHOZONE_API FEZHealthStatics
{
public:
	static bool IsCriticalBodyPart(EEZBodyPart Part);
	static bool IsLimb(EEZBodyPart Part);
	static bool IsArm(EEZBodyPart Part);
	static bool IsLeg(EEZBodyPart Part);

	static bool CanBleedBodyPart(EEZBodyPart Part);
	static bool CanFractureBodyPart(EEZBodyPart Part);

	static float GetDefaultMaxHealthForBodyPart(EEZBodyPart Part, const FEZHealthSystemSettings& Settings);

	static EEZContaminationStage ResolveContaminationStage(float Value, float Stage1Max, float Stage2Max);

	static float ClampNeedValue(float Value, float MaxValue);

	static FText GetBodyPartText(EEZBodyPart Part);
};