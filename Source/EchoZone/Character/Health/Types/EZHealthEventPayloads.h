#pragma once

#include "CoreMinimal.h"
#include "EZHealthTypes.h"
#include "EZHealthModifiers.h"
#include "EZHealthEventPayloads.generated.h"

USTRUCT(BlueprintType)
struct FEZBodyPartDamagedEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEZBodyPart BodyPart = EEZBodyPart::Torso;

	UPROPERTY(BlueprintReadOnly)
	float OldHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float NewHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float DamageApplied = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bDestroyedNow = false;
};

USTRUCT(BlueprintType)
struct FEZLifeStateChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEZLifeState OldState = EEZLifeState::Alive;

	UPROPERTY(BlueprintReadOnly)
	EEZLifeState NewState = EEZLifeState::Alive;
};

USTRUCT(BlueprintType)
struct FEZNeedChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEZNeedType NeedType = EEZNeedType::Hydration;

	UPROPERTY(BlueprintReadOnly)
	float OldValue = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float NewValue = 0.0f;
};

USTRUCT(BlueprintType)
struct FEZStatusEffectChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEZStatusEffectType EffectType = EEZStatusEffectType::None;

	UPROPERTY(BlueprintReadOnly)
	bool bAdded = false;

	UPROPERTY(BlueprintReadOnly)
	float TimeRemaining = 0.0f;
};

USTRUCT(BlueprintType)
struct FEZModifiersChangedEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FEZHealthModifiers Modifiers;
};
