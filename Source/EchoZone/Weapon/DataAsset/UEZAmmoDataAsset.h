// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "EchoZone/Weapon/Enum/EEZAmmoCaliber.h"

#include "UEZAmmoDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ECHOZONE_API UEZAmmoDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	FText AmmoName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	EEZAmmoCaliber Caliber = EEZAmmoCaliber::Cal_545x39;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float Damage = 35.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float Penetration = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float MuzzleVelocity = 80000.0f; // см/с

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float GravityScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float DragCoefficient = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistics")
	float MaxLifetime = 6.0f;
};
