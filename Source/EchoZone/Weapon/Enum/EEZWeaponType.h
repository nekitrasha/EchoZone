// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EEZWeaponType.generated.h"

UENUM(BlueprintType)
enum class EEZFireMode : uint8
{
	SemiAuto UMETA(DisplayName = "Semi Auto"),
	FullAuto UMETA(DisplayName = "Full Auto")
};

UENUM(BlueprintType)
enum class EEZWeaponType : uint8
{
	None UMETA(DisplayName = "None"),
	Pistol UMETA(DisplayName = "Pistol"),
	SMG UMETA(DisplayName = "SMG"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Rifle UMETA(DisplayName = "Rifle"),
	Carbine UMETA(DisplayName = "Carbine"),
	DMR UMETA(DisplayName = "DMR"),
	Sniper UMETA(DisplayName = "Shiper"),
	LMG UMETA(DisplayName = "LMG"),
	Launcher UMETA(DisplayName = "Launcher"),
	Special UMETA(DisplayName = "Special")
};
