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
};

