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
	bool bTriggerHeld = false;

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

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	USceneComponent* GetMuzzlePoint() const { return MuzzlePoint; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UEZWeaponDataAsset* GetWeaponData() const { return WeaponData; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZWeaponType GetWeaponType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZAmmoCaliber GetWeaponCaliber() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZReloadType GetReloadType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EEZWeaponFeedType GetFeedType() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	EEZFireMode GetCurrentFireMode() const { return CurrentFireMode; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsMagazineCompatible(const UEZMagazineDataAsset* MagazineData) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsAmmoCompatible(const UEZAmmoDataAsset* AmmoData) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetCurrentMagazineAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetAmmoReadyToFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetMagazineCapacity() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool HasInsertedMagazine() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool HasSpareMagazine() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	void SwitchFireMode();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool HasRoundChambered() const { return bRoundChambered; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnEmptyClick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnReloadStarted(bool bTacticalReload);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnReloadCanceled();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnReloadFinished();

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnFireModeChanged(EEZFireMode NewMode);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|FX")
	void BP_OnShotFired();

protected:
	void InitializeFromData();
	void FireShot();
	void HandleAutoFire();
	void FinishReload();
	void ConsumeRound();

	void HandleFireModeShot();
	void HandleBurstProgress();
	bool CanShootCurrentMode() const;
	bool IsFireModeSupported(EEZFireMode FireMode) const;

	void PlayEmptyClick();
	bool IsTacticalReload() const;
	bool NeedsChamberingAfterReload() const;

	void UpdateSpread(float DeltaSeconds);
	void UpdateRecoil(float DeltaSeconds);
	void UpdateADS(float DeltaSeconds);

	FVector GetCameraAimPoint() const;
	FVector GetShotDirection(const FVector& FromLocation) const;

	void ApplyRecoil();

	float GetErgonomicsNormalized() const;
	float GetMovementSpreadMultiplier() const;
	float GetEffectiveBaseSpread() const;
	float GetEffectiveVerticalRecoil() const;
	float GetEffectiveHorizontalRecoil() const;
	bool IsOwnerSprinting() const;

	float GetSecondsPerShot() const;
	float GetReloadDuration() const;

	bool TryChamberedNextRound();
	int32 FindBestMagazineIndex() const;
	void InsertMagazine(const FEZMagazineInstance& NewMagazine);
	FEZMagazineInstance RemoveInsertedMagazine();
};