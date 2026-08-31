// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Enum/EEZAmmoCaliber.h"

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float Damage = 35.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float MaxLifeTime = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float MuzzleVelocity = 80000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float GravityScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float DragCoefficient = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ballistic")
	float Penetration = 20.0f;
};
