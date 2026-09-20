#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "GameplayTagContainer.h"
#include "EZDamageContext.generated.h"

USTRUCT(BlueprintType)
struct FEZDamageContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	EEZBodyPart BodyPart = EEZBodyPart::Torso;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsBullet = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsExplosion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsElectric = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsPoison = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bIsMelee = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bCanCauseBleeding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bCanCauseFracture = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bCanCauseConcussion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	bool bCanCausePainShock = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float ImpulseSeverity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FGameplayTagContainer DamageTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TObjectPtr<AActor> DamageCauser = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TObjectPtr<AController> InstigatorController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	FHitResult HitResult;
};