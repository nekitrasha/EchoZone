#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AEZCharacter.generated.h"

class UCameraComponent;
class USceneComponent;
class UInputMappingContext;
class UInputAction;
class AEZWeaponBase;
class UEZCharacterMovementComponent;
class UEZStaminaComponent;
class UEZIntercatComponent;
class UEZHealthComponent;
class UEZInteractWidget;
class UEZWeaponPresentationComponent;

UCLASS()
class ECHOZONE_API AEZCharacter :public ACharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

};