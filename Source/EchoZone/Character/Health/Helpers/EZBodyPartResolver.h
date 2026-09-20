#pragma once

#include "CoreMinimal.h"
#include "EchoZone/Character/Health/Types/EZHealthTypes.h"

class ECHOZONE_API FEZBodyPartResolver
{
public:
	static EEZBodyPart ResolveFromBoneName(const FName& BoneName);
	static EEZBodyPart ResolveFromHitResult(const FHitResult& Hit);

private:
	static bool IsLeftBone(const FString& BoneLower);
};