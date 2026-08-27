// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UEZMagazineDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ECHOZONE_API UEZMagazineDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine")
	FText MagazineName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine")
	int32 Capacity = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine")
	float ReloadTime = 2.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine")
	float EmpryReloadTime = 2.8f;
};
