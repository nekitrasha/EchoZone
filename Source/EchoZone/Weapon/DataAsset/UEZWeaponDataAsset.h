// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "EchoZone/Weapon/Enum/EEZAmmoCaliber.h"
#include "EchoZone/Weapon/Enum/EEZFireMode.h"
#include "EchoZone/Weapon/Enum/EEZReloadType.h"
#include "EchoZone/Weapon/Enum/EEZWeaponFeedType.h"
#include "EchoZone/Weapon/Enum/EEZWeaponType.h"

#include "UEZWeaponDataAsset.generated.h"

class UEZAmmoDataAsset;
class UEZMagazineDataAsset;
class AEZProjectile;

USTRUCT(BlueprintType)
struct FEZWeaponSpreadSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float BaseSpreadAngle = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float SpreadPerShot = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float MaxSpreadAngle = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float RecoverySpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float MovingMultiplier = 1.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float CrouchMultiplier = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float ADSMultiplier = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float InAirMultiplier = 2.0f;
};

USTRUCT(BlueprintType)
struct FEZWeaponRecoilSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VerticalRecoil = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float HorizontalRecoil = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float RecoilKickPerShot = 0.08f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float MaxRecoilMultiplier = 1.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float RecoverySpeed = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VisualKickLocation = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VisualKickRotationPitch = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VisualKickRotationYaw = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VisualRecoverySpeed = 14.0f;
};

USTRUCT(BlueprintType)
struct FEZWeaponSwaySettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sway")
	float LocationSwayMultiplier = 0.015f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sway")
	float RotationSwayMultiplier = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sway")
	float InterpSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sway")
	float MaxLocationOffset = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sway")
	float MaxRotationOffset = 3.0f;
};

USTRUCT(BlueprintType)
struct FEZWeaponADSSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	float AimFOV = 70.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	float EnterTime = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	float ExitTime = 0.14f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	FVector ADSOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	FRotator ADSRotationOffset = FRotator::ZeroRotator;
};

UCLASS(BlueprintType)
class ECHOZONE_API UEZWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZWeaponType WeaponType = EEZWeaponType::Rifle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZAmmoCaliber Caliber = EEZAmmoCaliber::Cal_545x39;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZReloadType ReloadType = EEZReloadType::MagazineSwap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZWeaponFeedType FeedType = EEZWeaponFeedType::DetachableMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TArray<EEZFireMode> SupportedFireModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZFireMode DefaultFireMode = EEZFireMode::SemiAuto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 BurstCount = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRateRPM = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bCanFireWhileSprinting = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bAllowReloadCancel = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bSupportsTacticalReload = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Ergonomics = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEZProjectile> ProjectileClassOverride;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZAmmoDataAsset> DefaultAmmo = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZMagazineDataAsset> DefaultMagazine = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FEZWeaponSpreadSettings Spread;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FEZWeaponRecoilSettings Recoil;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FEZWeaponSwaySettings Sway;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FEZWeaponADSSettings ADS;
};
