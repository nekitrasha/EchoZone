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
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzlePiont;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EEZFireMode FireMode = EEZFireMode::SemiAuto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZAmmoDataAsset> AmmoData;

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



	FTimerHandle FireCooldownTimerHandle;
	FTimerHandle ReloadTimerHandle;

public:	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32  GetMagazineSize() const { return MagazineSize; }

protected:
	void FireShot();
	void ResetFire();
	void FinishReload();
};
