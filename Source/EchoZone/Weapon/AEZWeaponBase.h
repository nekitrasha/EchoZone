// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Enum/EEZWeaponType.h"
#include "Enum/EEZFireMode.h"
#include "Enum/EEZAmmoCaliber.h"
#include "Enum/EEZReloadType.h"
#include "Enum/EEZWeaponFeedType.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EEZFireMode CurrentFireMode = EEZFireMode::SemiAuto;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentMagazineAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 ReserveAmmo = 90;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bRoundChambered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bTriggerHeld = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float CurrentSpreadAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float CurrentRecoilMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	float CurrentAimAlpha = 0.0f;
	
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
	int32 GetAmmoInMagazine() const { return CurrentMagazineAmmo + (bRoundChambered ? 1 : 0); }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32  GetReserveAmmo() const { return ReserveAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetAimFOV() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USceneComponent* GetWeaponVisualRoot() const { return WeaponVisualRoot; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZAmmoCaliber GetWeaponCaliber() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZReloadType GetReloadType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZWeaponFeedType GetFeedType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsMagazineCompatible(const UEZMagazineDataAsset* MagazineData) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsAmmoCompatible(const UEZAmmoDataAsset* AmmoData) const;

protected:
	void InitializeFromData();
	void FireShot();
	void HandleAutoFire();
	void FinishReload();
	void ConsumeRound();
	
	void UpdateSpread(float DeltaSeconds);
	void UpdateRecoil(float DeltaSeconds);
	void UpdateADS(float DeltaSeconds);
	void UpdateVisualOffset(float DeltaSeconds);

	FVector GetCameraAimPoint() const;
	FVector GetShotDirection(const FVector& FromLocation) const;

	void ApplyRecoil();

	float GetErgonomicsNormalized() const;
	float GetMovementSpreadMultiplier() const;
	float GetEffectiveBaseSpread() const;
	float GetEffectiveVerticalRecoil() const;
	float GetEffectiveHorizontalRecoil() const;
	bool IsOwnerSprinting() const;

	int32 GetMagazineCapacity() const;
	float GetReloadDuration()  const;
};
