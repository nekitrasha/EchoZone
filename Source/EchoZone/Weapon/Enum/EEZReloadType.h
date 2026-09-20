// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEZReloadType.generated.h"


UENUM(BlueprintType)
enum class EEZReloadType : uint8
{
	None UMETA(DisplayName = "None"),
	MagazineSwap UMETA(DisplayName = "Magazine Swap"),
	InternalMagazine UMETA(DisplayName = "Internal Magazine"),
	SingleShell UMETA(DisplayName = "Single Shell"),
	Cylinder UMETA(DisplayName = "Cylinder"),
	ChargeBased UMETA(DisplayName = "Charge Based")	
};
