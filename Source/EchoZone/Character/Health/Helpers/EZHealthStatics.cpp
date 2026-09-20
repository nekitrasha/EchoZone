#include "EZHealthStatics.h"

bool FEZHealthStatics::IsCriticalBodyPart(EEZBodyPart Part)
{
	return Part == EEZBodyPart::Head || Part == EEZBodyPart::Torso;
}

bool FEZHealthStatics::IsLimb(EEZBodyPart Part)
{
	return Part == EEZBodyPart::LeftArm ||
		Part == EEZBodyPart::RightArm ||
		Part == EEZBodyPart::LeftLeg ||
		Part == EEZBodyPart::RightLeg;
}

bool FEZHealthStatics::IsArm(EEZBodyPart Part)
{
	return Part == EEZBodyPart::LeftArm || Part == EEZBodyPart::RightArm;
}

bool FEZHealthStatics::IsLeg(EEZBodyPart Part)
{
	return Part == EEZBodyPart::LeftLeg || Part == EEZBodyPart::RightLeg;
}

bool FEZHealthStatics::CanBleedBodyPart(EEZBodyPart Part)
{
	return Part == EEZBodyPart::Head ||
		Part == EEZBodyPart::Torso ||
		Part == EEZBodyPart::LeftArm ||
		Part == EEZBodyPart::RightArm ||
		Part == EEZBodyPart::LeftLeg ||
		Part == EEZBodyPart::RightLeg;
}

bool FEZHealthStatics::CanFractureBodyPart(EEZBodyPart Part)
{
	return IsLimb(Part);
}

float FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart Part, const FEZHealthSystemSettings& Settings)
{
	return IsCriticalBodyPart(Part) ? Settings.CriticalPartMaxHealth : Settings.LimbMaxHealth;
}

EEZContaminationStage FEZHealthStatics::ResolveContaminationStage(float Value, float Stage1Max, float Stage2Max)
{
	if (Value <= 0.0f)
	{
		return EEZContaminationStage::None;
	}

	if (Value <= Stage1Max)
	{
		return EEZContaminationStage::Stage1;
	}

	if (Value <= Stage2Max)
	{
		return EEZContaminationStage::Stage2;
	}

	return EEZContaminationStage::Stage3;
}

float FEZHealthStatics::ClampNeedValue(float Value, float MaxValue)
{
	return FMath::Clamp(Value, 0.0f, MaxValue);
}

FText FEZHealthStatics::GetBodyPartText(EEZBodyPart Part)
{
	switch (Part)
	{
	case EEZBodyPart::Head:
		return FText::FromString(TEXT("Head"));
	case EEZBodyPart::Torso:
		return FText::FromString(TEXT("Torso"));
	case EEZBodyPart::LeftArm:
		return FText::FromString(TEXT("Left Arm"));
	case EEZBodyPart::RightArm:
		return FText::FromString(TEXT("Right Arm"));
	case EEZBodyPart::LeftLeg:
		return FText::FromString(TEXT("Left Leg"));
	case EEZBodyPart::RightLeg:
		return FText::FromString(TEXT("Right Leg"));
	default:
		return FText::FromString(TEXT("Unknown"));
	}
}
