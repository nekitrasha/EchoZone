// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EchoZone/Weapon/Enum/EEZWeaponType.h"
#include "AEZWeaponBase.generated.h"

class UStaticMeshComponent;
class USceneRootComponent;
class AEZProjectile;
class UEZAmmoDataAsset;
class UEZWeaponDataAsset;
class UEZMagazineDataAsset;

UCLASS()
class ECHOZONE_API AEZWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootScene;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* WeaponVisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzlePoint;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZWeaponDataAsset> WeaponData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZMagazineDataAsset> CurrentMagazineData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZAmmoDataAsset> CurrentAmmoData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZFireMode FireMode = EEZFireMode::SemiAuto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 MagazineSize = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 AmmoInMagazine = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 ReserveAmmo = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ReloadTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bCanFireWhileSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bTriggerHeld = false;
	
	//Spread
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float BaseSpreadAngle = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float SpreadPerShot = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float MaxSpreadAngle = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float SpreadRecoverySpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float MovingSpreadMultiplier = 1.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float CrouchSpreadMultiplier = 0.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float ADS_SpreadMultiplier = 0.65f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spread")
	float InAirSpreadMultiplier = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spread")
	float CurrentSpreadAngle = 1.0f;

	//RECOIL
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float VerticalRecoil = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float HorizontalRecoil = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float RecoilKickPerShot = 0.08f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float MaxRecoilMultiplier = 1.75f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Recoil")
	float CurrentRecoilMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recoil")
	float RecoilRecoverySpeed = 4.0f;

	//Ergonomic
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ergonomic")
	float Ergonomics = 50.0f;

	//ADS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ADS")
	bool bIsAiming = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	float AimEnterTime = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ADS")
	float AimExitTime = 0.14f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebugShot = false;

	FTimerHandle AutoFireTimerHandle;
	FTimerHandle ReloadTimerHandle;

public:	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartAim();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopAim();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32  GetReserveAmmo() const { return ReserveAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetAimAlpha() const;

protected:
	void FireShot();
	void HandleAutoFire();
	void FinishReload();

	FVector GetAimDirection() const;
	FVector GetShotDirection(const FVector& FromLocation) const;
	FVector GetCameraAimPoint() const;

	void ApplyRecoil();

	float GetErgonomicsNormalized() const;
	float GetMovementSpreadMultiplier() const;
	float GetEffectiveBaseSpread() const;
	float GetEffectiveVerticalRecoil() const;
	float GetEffectiveHorizontalRecoil() const;
	bool IsOwnerSprinting() const;
};
