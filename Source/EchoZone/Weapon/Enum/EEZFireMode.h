// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEZFireMode.generated.h"

UENUM(BlueprintType)
enum class EEZFireMode : uint8
{
	Safe UMETA(DisplayName = "Safe"),
	SemiAuto UMETA(DisplayName = "Semi Auto"),
	Burst UMETA(DisplayName = "Burst"),
	FullAuto UMETA(DisplayName = "Full Auto")
};

