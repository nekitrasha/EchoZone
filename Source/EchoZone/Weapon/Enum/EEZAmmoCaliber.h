// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EEZAmmoCaliber.generated.h"

UENUM(BlueprintType)
enum class EEZAmmoCaliber : uint8
{
	None UMETA(DisplayName = "None"),

	Cal_9x19 UMETA(DisplayName = "9x19mm"),
	Cal_45ACP UMETA(DisplayName = ".45 ACP"),
	Cal_357 UMETA(DisplayName = ".357"),
	Cal_12Gauge UMETA(DisplayName = "12 Gauge"),

	Cal_545x39 UMETA(DisplayName = "5.45x39mm"),
	Cal_556x45 UMETA(DisplayName = "5.56x45mm"),
	Cal_762x39 UMETA(DisplayName = "7.62x39mm"),
	Cal_762x51 UMETA(DisplayName = "7.62x51mm"),
	Cal_762x54 UMETA(DisplayName = "7.62x54R"),
	Cal_127x99 UMETA(DisplayName = "12.7x99mm"),

	Rocket UMETA(DisplayName = "Rocket"),
	Grenade40mm UMETA(DisplayName = "40mm Grenade"),
	Special UMETA(DisplayName = "Special")
};

