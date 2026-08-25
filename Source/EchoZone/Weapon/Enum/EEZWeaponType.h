// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EEZWeaponType.generated.h"

UENUM(BlueprintType)
enum class EEZFireMode : uint8
{
	SemiAuto UMETA(DisplayName = "Semi Auto"),
	FullAuto UMETA(DisplayName = "Full Auto")
};
