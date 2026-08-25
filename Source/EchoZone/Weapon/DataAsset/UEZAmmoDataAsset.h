// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
	float Damage = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	float InitialSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	float MaxSpeed = 4000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	float GravityScale = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	float LifeSeconds = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	float PenetrationPower = 0.0f;
};
