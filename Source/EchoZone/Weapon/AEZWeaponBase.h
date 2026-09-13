#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Enum/EEZWeaponType.h"
#include "Enum/EEZFireMode.h"
#include "Enum/EEZAmmoCaliber.h"
#include "Enum/EEZReloadType.h"
#include "Enum/EEZWeaponFeedType.h"
#include "Struct/FEZMagazineInstance.h"

#include "AEZWeaponBase.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class AEZProjectile;
class UEZAmmoDataAsset;
class UEZWeaponDataAsset;
class UEZMagazineDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZWeaponSimpleSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZWeaponReloadStartedSignature, bool, bTacticalReload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZWeaponFireModeChangedSignature, EEZFireMode, NewMode);

UCLASS()
class ECHOZONE_API AEZWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AEZWeaponBase();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> WeaponVisualRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> MuzzlePoint;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AEZProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZWeaponDataAsset> WeaponData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UEZAmmoDataAsset> CurrentAmmoData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EEZFireMode CurrentFireMode = EEZFireMode::SemiAuto;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FEZMagazineInstance InsertedMagazine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<FEZMagazineInstance> MagazineInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bRoundChambered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bTriggeredHeld = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bWasEmptyClick = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	int32 BurstShotRemaining = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bTacticalReloadInProgress = false;

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
	FTimerHandle EmptyClickCooldownHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FEZWeaponSimpleSignature OnShotFiredEvent;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FEZWeaponSimpleSignature OnEmptyClickEvent;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FEZWeaponReloadStartedSignature OnReloadStartedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FEZWeaponSimpleSignature OnReloadFinishedEvent;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FEZWeaponFireModeChangedSignature OnFireModeChangedEvent;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Reload")
	void CancelReload();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StartAim();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void StopAim();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	float GetAimFOV() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USceneComponent* GetWeaponVisualRoot() const { return WeaponVisualRoot; }


};