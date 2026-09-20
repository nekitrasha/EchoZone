// Fill out your copyright notice in the Description page of Project Settings.


#include "EZBodyPartResolver.h"

bool FEZBodyPartResolver::IsLeftBone(const FString& BoneLower)
{
	return BoneLower.Contains(TEXT("l_")) ||
		BoneLower.Contains(TEXT("_l")) ||
		BoneLower.Contains(TEXT("left"));
}

EEZBodyPart FEZBodyPartResolver::ResolveFromBoneName(const FName& BoneName)
{
	const FString Bone = BoneName.ToString().ToLower();

	if (Bone.Contains(TEXT("head")) || Bone.Contains(TEXT("neck")))
	{
		return EEZBodyPart::Head;
	}

	if (Bone.Contains(TEXT("spine")) || Bone.Contains(TEXT("pelvis")) || Bone.Contains(TEXT("chest")) || Bone.Contains(TEXT("torso")))
	{
		return EEZBodyPart::Torso;
	}

	if (Bone.Contains(TEXT("upperarm")) || Bone.Contains(TEXT("lowerarm")) || Bone.Contains(TEXT("hand")) || Bone.Contains(TEXT("clavicle")))
	{
		return IsLeftBone(Bone) ? EEZBodyPart::LeftArm : EEZBodyPart::RightArm;
	}

	if (Bone.Contains(TEXT("thigh")) || Bone.Contains(TEXT("calf")) || Bone.Contains(TEXT("foot")) || Bone.Contains(TEXT("ball")))
	{
		return IsLeftBone(Bone) ? EEZBodyPart::LeftLeg : EEZBodyPart::RightLeg;
	}

	return EEZBodyPart::Torso;
}

EEZBodyPart FEZBodyPartResolver::ResolveFromHitResult(const FHitResult& Hit)
{
	return ResolveFromBoneName(Hit.BoneName);
}