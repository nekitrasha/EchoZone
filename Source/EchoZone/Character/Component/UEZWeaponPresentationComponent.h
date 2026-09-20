#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEZWeaponPresentationComponent.generated.h"

class AEZWeaponBase;
class USceneComponent;
class UEZWeaponDataAsset;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOZONE_API UEZWeaponPresentationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEZWeaponPresentationComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon Presentation")
	void SetCurrentWeapon(AEZWeaponBase* InWeapon);

	UFUNCTION(BlueprintCallable, Category = "Weapon Presentation")
	void SetADSAnchor(USceneComponent* InAnchor);

	UFUNCTION(BlueprintCallable, Category = "Weapon Presentation")
	void SetAiming(bool bNewAiming);

	UFUNCTION()
	void NotifyShotFired();

protected:
	UPROPERTY()
	TObjectPtr<AEZWeaponBase> CurrentWeapon = nullptr;

	UPROPERTY()
	TObjectPtr<USceneComponent> ADSAnchor = nullptr;

	UPROPERTY()
	bool bIsAiming = false;

	UPROPERTY()
	float CurrentAimAlpha = 0.0f;

	UPROPERTY()
	FVector VisualLocationOffset = FVector::ZeroVector;

	UPROPERTY()
	FRotator VisualRotationOffset = FRotator::ZeroRotator;

	UPROPERTY()
	FVector TargetVisualLocationOffset = FVector::ZeroVector;

	UPROPERTY()
	FRotator TargetVisualRotationOffset = FRotator::ZeroRotator;

	UPROPERTY()
	FVector SwayLocationOffset = FVector::ZeroVector;

	UPROPERTY()
	FRotator SwayRotationOffset = FRotator::ZeroRotator;

	UPROPERTY()
	FVector ADSLocationOffset = FVector::ZeroVector;

	UPROPERTY()
	FRotator ADSRotationOffset = FRotator::ZeroRotator;

	UPROPERTY()
	FRotator LastControlRotation = FRotator::ZeroRotator;

protected:
	void UpdateAimAlpha(float DeltaTime);
	void UpdateVisualRecoil(float DeltaTime);
	void UpdateWeaponSway(float DeltaTime);
	void UpdateADSAlignment(float DeltaTime);
	void ApplyPresentationOffset();
	void ResetPresentation();

	const UEZWeaponDataAsset* GetWeaponData() const;
	void BindToWeapon(AEZWeaponBase* InWeapon);
	void UnbindFromWeapon(AEZWeaponBase* InWeapon);
};

