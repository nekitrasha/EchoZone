#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EchoZone/Character/Health/Types/EZHealthTypes.h"
#include "EchoZone/Character/Health/Types/EZDamageContext.h"
#include "EchoZone/Character/Health/Types/EZBodyPartState.h"
#include "EchoZone/Character/Health/Types/EZNeedState.h"
#include "EchoZone/Character/Health/Types/EZStatusEffectState.h"
#include "EchoZone/Character/Health/Types/EZContaminationState.h"
#include "EchoZone/Character/Health/Types/EZHealthModifiers.h"
#include "EchoZone/Character/Health/Types/EZHealthSnapshot.h"
#include "EchoZone/Character/Health/Types/EZHealthEventPayloads.h"
#include "EchoZone/Character/Health/Items/EZMedicalActionContext.h"
#include "EchoZone/Character/Health/Items/EZMedicalActionTypes.h"
#include "EchoZone/Character/Health/Data/EZHealthSystemSettings.h"
#include "EchoZone/Character/Health/Data/EZHealthSystemSettingsDA.h"
#include "UEZHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZOnBodyPartDamagedSimple, const FEZBodyPartDamagedEvent&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZOnLifeStateChangedSimple, const FEZLifeStateChangedEvent&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZOnNeedChangedSimple, const FEZNeedChangedEvent&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZOnStatusEffectChangedSimple, const FEZStatusEffectChangedEvent&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEZOnModifiersChangedSimple, const FEZModifiersChangedEvent&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZOnDeathSimple);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZOnDropWeaponRequestedSimple);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZOnScreamRequestedSimple);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZOnAdrenalineStartedSimple);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEZOnAdrenalineEndedSimple);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECHOZONE_API UEZHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEZHealthComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnBodyPartDamagedSimple OnBodyPartDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnLifeStateChangedSimple OnLifeStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnNeedChangedSimple OnNeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnStatusEffectChangedSimple OnStatusEffectChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnModifiersChangedSimple OnModifiersChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnDeathSimple OnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnDropWeaponRequestedSimple OnDropWeaponRequested;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnScreamRequestedSimple OnScreamRequested;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnAdrenalineStartedSimple OnAdrenalineStarted;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FEZOnAdrenalineEndedSimple OnAdrenalineEnded;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Config")
	TObjectPtr<UEZHealthSystemSettingsDA> SettingsAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Config")
	FEZHealthSystemSettings Settings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState HeadState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState TorsoState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState LeftArmState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState RightArmState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState LeftLegState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|BodyParts")
	FEZBodyPartState RightLegState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	EEZLifeState LifeState = EEZLifeState::Alive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Needs")
	FEZNeedState Hydration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Needs")
	FEZNeedState Hunger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Needs")
	float BodyTemperature = 36.6f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Contamination")
	FEZContaminationState BiologicalContamination;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Contamination")
	FEZContaminationState RadiationContamination;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Status")
	TArray<FEZStatusEffectState> ActiveStatusEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Adrenaline")
	bool bAdrenalineActive = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Adrenaline")
	float AdrenalineTimeRemaining = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Adrenaline")
	bool bDeferredPainShock = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Modifiers")
	FEZHealthModifiers CachedModifiers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Snapshot")
	FEZHealthSnapshot CachedSnapshot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|OwnerState")
	bool bOwnerMoving = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|OwnerState")
	bool bOwnerSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|OwnerState")
	EEZBodyStance OwnerStance = EEZBodyStance::Standing;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(const FEZDamageContext& DamageContext);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void HealBodyPart(EEZBodyPart BodyPart, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void RestoreAllBodyParts(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health|Needs")
	void AddHydration(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health|Needs")
	void AddHunger(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health|Needs")
	void ModifyBodyTemperature(float DeltaTemperature);

	UFUNCTION(BlueprintCallable, Category = "Health|Trauma")
	void CureBleeding(EEZBodyPart BodyPart, EEZBleedSeverity MaxSeverityCured);

	UFUNCTION(BlueprintCallable, Category = "Health|Trauma")
	void CureFracture(EEZBodyPart BodyPart);

	UFUNCTION(BlueprintCallable, Category = "Health|Trauma")
	void ExtinguishBurning(EEZBodyPart BodyPart);

	UFUNCTION(BlueprintCallable, Category = "Health|Trauma")
	void RemovePoison();

	UFUNCTION(BlueprintCallable, Category = "Health|Status")
	void ApplyStatusEffect(const FEZStatusEffectState& Effect);

	UFUNCTION(BlueprintCallable, Category = "Health|Status")
	void RemoveStatusEffect(EEZStatusEffectType EffectType);

	UFUNCTION(BlueprintCallable, Category = "Health|Contamination")
	void AddContamination(EEZContaminationType Type, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health|Contamination")
	void ReduceContamination(EEZContaminationType Type, float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health|Medical")
	EEZMedicalItemUseResult ApplyMedicalAction(const FEZMedicalActionContext& Context);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void NotifyHardLanding(float Severity);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void NotifyNearExplosion(float Severity);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void NotifyElectricExposure(float DurationOverride = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMovementStateSnapshot(bool bMoving, bool bSprinting, EEZBodyStance Stance);

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const { return LifeState != EEZLifeState::Dead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return LifeState == EEZLifeState::Dead; }

	UFUNCTION(BlueprintPure, Category = "Health")
	EEZLifeState GetLifeState() const { return LifeState; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FEZHealthModifiers GetHealthModifiers() const { return CachedModifiers; }

	UFUNCTION(BlueprintPure, Category = "Health")
	FEZHealthSnapshot GetHealthSnapshot() const { return CachedSnapshot; }

	UFUNCTION(BlueprintPure, Category = "Health")
	const FEZBodyPartState& GetBodyPartState(EEZBodyPart BodyPart) const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHydrationPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHungerPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetBodyTemperature() const { return BodyTemperature; }

protected:
	FEZBodyPartState& GetBodyPartStateRef(EEZBodyPart BodyPart);
	const FEZBodyPartState& GetBodyPartStateConstRef(EEZBodyPart BodyPart) const;

	FEZContaminationState& GetContaminationStateRef(EEZContaminationType Type);

	void ApplyDamageToBodyPart(EEZBodyPart BodyPart, float Damage);
	void ApplyOverflowDamageToTorso(float Damage);
	void ApplyDirectTickDamage(EEZBodyPart BodyPart, float Damage);

	void UpdateLifeState();
	void UpdateCachedModifiers();
	void UpdateCachedSnapshot();

	void ProcessNeeds(float DeltaTime);
	void ProcessBleeding(float DeltaTime);
	void ProcessStatusEffects(float DeltaTime);
	void ProcessContamination(float DeltaTime);
	void ProcessAdrenaline(float DeltaTime);

	void TriggerPainShockIfNeeded(const FEZDamageContext& DamageContext, float DamageApplied, bool bBodyPartDestroyed);
	void StartAdrenaline(float Duration);

	void UpdateContaminationStage(FEZContaminationState& State, EEZContaminationType Type);
	void KillOwner();

	void InitializeBodyPart(FEZBodyPartState& State, float MaxHealthValue);

	bool HasStatusEffect(EEZStatusEffectType EffectType) const;
	int32 FindStatusEffectIndex(EEZStatusEffectType EffectType) const;

	void BroadcastModifiersChanged();
	void BroadcastNeedChanged(EEZNeedType NeedType, float OldValue, float NewValue);
};