#include "UEZHealthComponent.h"
#include "EchoZone/Character/Health/Helpers/EZHealthStatics.h"
#include "GameFramework/Actor.h"

UEZHealthComponent::UEZHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Hydration.MaxValue = 100.0f;
	Hydration.CurrentValue = 100.0f;
	Hydration.DecayPerSecond = 0.03f;

	Hunger.MaxValue = 100.0f;
	Hunger.CurrentValue = 100.0f;
	Hunger.DecayPerSecond = 0.01f;

	BodyTemperature = 36.6f;
}

void UEZHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (SettingsAsset)
	{
		Settings = SettingsAsset->Settings;
	}

	Hydration.DecayPerSecond = Settings.HydrationDecayPerSecond;
	Hunger.DecayPerSecond = Settings.HungerDecayPerSecond;
	BodyTemperature = Settings.NormalBodyTemperature;

	InitializeBodyPart(HeadState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::Head, Settings));
	InitializeBodyPart(TorsoState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::Torso, Settings));
	InitializeBodyPart(LeftArmState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::LeftArm, Settings));
	InitializeBodyPart(RightArmState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::RightArm, Settings));
	InitializeBodyPart(LeftLegState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::LeftLeg, Settings));
	InitializeBodyPart(RightLegState, FEZHealthStatics::GetDefaultMaxHealthForBodyPart(EEZBodyPart::RightLeg, Settings));

	UpdateCachedModifiers();
	UpdateCachedSnapshot();
	UpdateLifeState();
}

void UEZHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LifeState == EEZLifeState::Dead)
	{
		return;
	}

	ProcessAdrenaline(DeltaTime);
	ProcessBleeding(DeltaTime);
	ProcessNeeds(DeltaTime);
	ProcessStatusEffects(DeltaTime);
	ProcessContamination(DeltaTime);

	UpdateCachedModifiers();
	UpdateCachedSnapshot();
	UpdateLifeState();
}

void UEZHealthComponent::InitializeBodyPart(FEZBodyPartState& State, float MaxHealthValue)
{
	State.MaxHealth = MaxHealthValue;
	State.CurrentHealth = MaxHealthValue;
	State.bDestroyed = false;
	State.bFractured = false;
	State.bBurned = false;
	State.BleedSeverity = EEZBleedSeverity::None;
	State.BurnDamageMultiplier = 1.0f;
	State.AutoClotProgress = 0.0f;
	State.AccumulatedPain = 0.0f;
	State.LastDamageGameTime = 0.0f;
}

FEZBodyPartState& UEZHealthComponent::GetBodyPartStateRef(EEZBodyPart BodyPart)
{
	switch (BodyPart)
	{
	case EEZBodyPart::Head: return HeadState;
	case EEZBodyPart::Torso: return TorsoState;
	case EEZBodyPart::LeftArm: return LeftArmState;
	case EEZBodyPart::RightArm: return RightArmState;
	case EEZBodyPart::LeftLeg: return LeftLegState;
	case EEZBodyPart::RightLeg: return RightLegState;
	default: return TorsoState;
	}
}

const FEZBodyPartState& UEZHealthComponent::GetBodyPartStateConstRef(EEZBodyPart BodyPart) const
{
	switch (BodyPart)
	{
	case EEZBodyPart::Head: return HeadState;
	case EEZBodyPart::Torso: return TorsoState;
	case EEZBodyPart::LeftArm: return LeftArmState;
	case EEZBodyPart::RightArm: return RightArmState;
	case EEZBodyPart::LeftLeg: return LeftLegState;
	case EEZBodyPart::RightLeg: return RightLegState;
	default: return TorsoState;
	}
}

const FEZBodyPartState& UEZHealthComponent::GetBodyPartState(EEZBodyPart BodyPart) const
{
	return GetBodyPartStateConstRef(BodyPart);
}

FEZContaminationState& UEZHealthComponent::GetContaminationStateRef(EEZContaminationType Type)
{
	return (Type == EEZContaminationType::Biological) ? BiologicalContamination : RadiationContamination;
}

float UEZHealthComponent::GetHydrationPercent() const
{
	return Hydration.MaxValue > 0.0f ? Hydration.CurrentValue / Hydration.MaxValue : 0.0f;
}

float UEZHealthComponent::GetHungerPercent() const
{
	return Hunger.MaxValue > 0.0f ? Hunger.CurrentValue / Hunger.MaxValue : 0.0f;
}

void UEZHealthComponent::ApplyDamage(const FEZDamageContext& DamageContext)
{
	if (LifeState == EEZLifeState::Dead || DamageContext.Damage <= 0.0f)
	{
		return;
	}

	FEZBodyPartState& PartState = GetBodyPartStateRef(DamageContext.BodyPart);

	if (DamageContext.bIsBullet && FEZHealthStatics::IsLimb(DamageContext.BodyPart) && PartState.bDestroyed)
	{
		ApplyOverflowDamageToTorso(DamageContext.Damage * Settings.BlackLimbOverflowMultiplier);
		UpdateCachedModifiers();
		UpdateCachedSnapshot();
		UpdateLifeState();
		return;
	}

	float FinalDamage = DamageContext.Damage;

	if (PartState.bBurned && (DamageContext.bIsBullet || DamageContext.bIsExplosion || DamageContext.bIsMelee))
	{
		FinalDamage *= Settings.BurnDamageMultiplier;
	}

	const float OldHealth = PartState.CurrentHealth;
	PartState.CurrentHealth = FMath::Clamp(PartState.CurrentHealth - FinalDamage, 0.0f, PartState.MaxHealth);
	PartState.AccumulatedPain += FinalDamage;
	PartState.LastDamageGameTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	const bool bDestroyedNow = !PartState.bDestroyed && PartState.CurrentHealth <= 0.0f;
	if (bDestroyedNow)
	{
		PartState.bDestroyed = true;
	}

	FEZBodyPartDamagedEvent DamageEvent;
	DamageEvent.BodyPart = DamageContext.BodyPart;
	DamageEvent.OldHealth = OldHealth;
	DamageEvent.NewHealth = PartState.CurrentHealth;
	DamageEvent.DamageApplied = FinalDamage;
	DamageEvent.bDestroyedNow = bDestroyedNow;
	OnBodyPartDamaged.Broadcast(DamageEvent);

	if (DamageContext.bCanCauseBleeding && FEZHealthStatics::CanBleedBodyPart(DamageContext.BodyPart))
	{
		const EEZBleedSeverity NewBleed = DamageContext.bIsExplosion ? EEZBleedSeverity::Heavy : EEZBleedSeverity::Light;
		PartState.BleedSeverity = (uint8)NewBleed > (uint8)PartState.BleedSeverity ? NewBleed : PartState.BleedSeverity;
		PartState.AutoClotProgress = 0.0f;
	}

	if (DamageContext.bCanCauseFracture && FEZHealthStatics::CanFractureBodyPart(DamageContext.BodyPart))
	{
		PartState.bFractured = true;
	}

	if (DamageContext.bIsFire)
	{
		PartState.bBurned = true;
		PartState.BurnDamageMultiplier = Settings.BurnDamageMultiplier;

		FEZStatusEffectState Burning;
		Burning.EffectType = EEZStatusEffectType::Burning;
		Burning.Duration = 15.0f * 60.0f;
		Burning.TimeRemaining = Burning.Duration;
		Burning.Intensity = 1.0f;
		Burning.SourceBodyPart = DamageContext.BodyPart;
		ApplyStatusEffect(Burning);
	}

	if (DamageContext.bIsPoison)
	{
		FEZStatusEffectState Poison;
		Poison.EffectType = EEZStatusEffectType::Poison;
		Poison.Duration = 10.0f * 60.0f;
		Poison.TimeRemaining = Poison.Duration;
		Poison.Intensity = 1.0f;
		Poison.SourceBodyPart = DamageContext.BodyPart;
		ApplyStatusEffect(Poison);
	}

	if (DamageContext.bCanCauseConcussion)
	{
		FEZStatusEffectState Concussion;
		Concussion.EffectType = EEZStatusEffectType::Concussion;
		Concussion.Duration = FMath::FRandRange(Settings.ConcussionMinDuration, Settings.ConcussionMaxDuration);
		Concussion.TimeRemaining = Concussion.Duration;
		Concussion.Intensity = 1.0f;
		Concussion.SourceBodyPart = DamageContext.BodyPart;
		ApplyStatusEffect(Concussion);
	}

	if (DamageContext.bIsElectric)
	{
		FEZStatusEffectState Electric;
		Electric.EffectType = EEZStatusEffectType::ElectricShock;
		Electric.Duration = Settings.ElectricShockDuration;
		Electric.TimeRemaining = Electric.Duration;
		Electric.Intensity = 1.0f;
		Electric.SourceBodyPart = DamageContext.BodyPart;
		ApplyStatusEffect(Electric);
	}

	if (FinalDamage > Settings.AdrenalineTriggerDamage)
	{
		StartAdrenaline(Settings.AdrenalineDuration);
	}

	TriggerPainShockIfNeeded(DamageContext, FinalDamage, bDestroyedNow);

	UpdateLifeState();
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::ApplyDamageToBodyPart(EEZBodyPart BodyPart, float Damage)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);

	const float OldHealth = PartState.CurrentHealth;
	PartState.CurrentHealth = FMath::Clamp(PartState.CurrentHealth - Damage, 0.0f, PartState.MaxHealth);

	const bool bDestroyedNow = !PartState.bDestroyed && PartState.CurrentHealth <= 0.0f;
	if (bDestroyedNow)
	{
		PartState.bDestroyed = true;
	}

	FEZBodyPartDamagedEvent Event;
	Event.BodyPart = BodyPart;
	Event.OldHealth = OldHealth;
	Event.NewHealth = PartState.CurrentHealth;
	Event.DamageApplied = Damage;
	Event.bDestroyedNow = bDestroyedNow;
	OnBodyPartDamaged.Broadcast(Event);
}

void UEZHealthComponent::ApplyOverflowDamageToTorso(float Damage)
{
	ApplyDamageToBodyPart(EEZBodyPart::Torso, Damage);
}

void UEZHealthComponent::ApplyDirectTickDamage(EEZBodyPart BodyPart, float Damage)
{
	if (LifeState == EEZLifeState::Dead || Damage <= 0.0f)
	{
		return;
	}

	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);

	if (PartState.bDestroyed && FEZHealthStatics::IsLimb(BodyPart))
	{
		ApplyOverflowDamageToTorso(Damage * Settings.BlackLimbOverflowMultiplier);
	}
	else
	{
		ApplyDamageToBodyPart(BodyPart, Damage);
	}

	UpdateLifeState();
}

void UEZHealthComponent::HealBodyPart(EEZBodyPart BodyPart, float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);
	const float OldHealth = PartState.CurrentHealth;
	PartState.CurrentHealth = FMath::Clamp(PartState.CurrentHealth + Amount, 0.0f, PartState.MaxHealth);

	if (PartState.CurrentHealth > 0.0f)
	{
		PartState.bDestroyed = false;
	}

	FEZBodyPartDamagedEvent Event;
	Event.BodyPart = BodyPart;
	Event.OldHealth = OldHealth;
	Event.NewHealth = PartState.CurrentHealth;
	Event.DamageApplied = 0.0f;
	Event.bDestroyedNow = false;
	OnBodyPartDamaged.Broadcast(Event);

	UpdateLifeState();
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::RestoreAllBodyParts(float Amount)
{
	HealBodyPart(EEZBodyPart::Head, Amount);
	HealBodyPart(EEZBodyPart::Torso, Amount);
	HealBodyPart(EEZBodyPart::LeftArm, Amount);
	HealBodyPart(EEZBodyPart::RightArm, Amount);
	HealBodyPart(EEZBodyPart::LeftLeg, Amount);
	HealBodyPart(EEZBodyPart::RightLeg, Amount);
}

void UEZHealthComponent::AddHydration(float Amount)
{
	const float OldValue = Hydration.CurrentValue;
	Hydration.CurrentValue = FEZHealthStatics::ClampNeedValue(Hydration.CurrentValue + Amount, Hydration.MaxValue);
	Hydration.bDepleted = Hydration.CurrentValue <= 0.0f;

	BroadcastNeedChanged(EEZNeedType::Hydration, OldValue, Hydration.CurrentValue);
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::AddHunger(float Amount)
{
	const float OldValue = Hunger.CurrentValue;
	Hunger.CurrentValue = FEZHealthStatics::ClampNeedValue(Hunger.CurrentValue + Amount, Hunger.MaxValue);
	Hunger.bDepleted = Hunger.CurrentValue <= 0.0f;

	BroadcastNeedChanged(EEZNeedType::Hunger, OldValue, Hunger.CurrentValue);
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::ModifyBodyTemperature(float DeltaTemperature)
{
	BodyTemperature += DeltaTemperature;
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::CureBleeding(EEZBodyPart BodyPart, EEZBleedSeverity MaxSeverityCured)
{
	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);

	if (PartState.BleedSeverity == EEZBleedSeverity::None)
	{
		return;
	}

	const bool bCanCure =
		(MaxSeverityCured == EEZBleedSeverity::Heavy) ||
		(MaxSeverityCured == EEZBleedSeverity::Light && PartState.BleedSeverity == EEZBleedSeverity::Light);

	if (bCanCure)
	{
		PartState.BleedSeverity = EEZBleedSeverity::None;
		PartState.AutoClotProgress = 0.0f;
		UpdateCachedModifiers();
		UpdateCachedSnapshot();
	}
}

void UEZHealthComponent::CureFracture(EEZBodyPart BodyPart)
{
	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);
	PartState.bFractured = false;
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::ExtinguishBurning(EEZBodyPart BodyPart)
{
	FEZBodyPartState& PartState = GetBodyPartStateRef(BodyPart);
	PartState.bBurned = false;
	PartState.BurnDamageMultiplier = 1.0f;

	const int32 Index = ActiveStatusEffects.IndexOfByPredicate([BodyPart](const FEZStatusEffectState& Effect)
		{
			return Effect.EffectType == EEZStatusEffectType::Burning && Effect.SourceBodyPart == BodyPart;
		});

	if (Index != INDEX_NONE)
	{
		ActiveStatusEffects.RemoveAt(Index);

		FEZStatusEffectChangedEvent Event;
		Event.EffectType = EEZStatusEffectType::Burning;
		Event.bAdded = false;
		Event.TimeRemaining = 0.0f;
		OnStatusEffectChanged.Broadcast(Event);
	}

	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::RemovePoison()
{
	RemoveStatusEffect(EEZStatusEffectType::Poison);
}

int32 UEZHealthComponent::FindStatusEffectIndex(EEZStatusEffectType EffectType) const
{
	return ActiveStatusEffects.IndexOfByPredicate([EffectType](const FEZStatusEffectState& Effect)
		{
			return Effect.EffectType == EffectType;
		});
}

bool UEZHealthComponent::HasStatusEffect(EEZStatusEffectType EffectType) const
{
	return FindStatusEffectIndex(EffectType) != INDEX_NONE;
}

void UEZHealthComponent::ApplyStatusEffect(const FEZStatusEffectState& Effect)
{
	const int32 ExistingIndex = FindStatusEffectIndex(Effect.EffectType);

	if (ExistingIndex != INDEX_NONE)
	{
		FEZStatusEffectState& Existing = ActiveStatusEffects[ExistingIndex];
		Existing.TimeRemaining = FMath::Max(Existing.TimeRemaining, Effect.TimeRemaining);
		Existing.Duration = FMath::Max(Existing.Duration, Effect.Duration);
		Existing.Intensity = FMath::Max(Existing.Intensity, Effect.Intensity);
		Existing.StackCount = FMath::Max(Existing.StackCount, Effect.StackCount);
	}
	else
	{
		ActiveStatusEffects.Add(Effect);

		FEZStatusEffectChangedEvent Event;
		Event.EffectType = Effect.EffectType;
		Event.bAdded = true;
		Event.TimeRemaining = Effect.TimeRemaining;
		OnStatusEffectChanged.Broadcast(Event);

		if (Effect.EffectType == EEZStatusEffectType::PainShock)
		{
			if (FMath::FRand() <= 0.5f)
			{
				OnDropWeaponRequested.Broadcast();
			}
		}
	}

	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::RemoveStatusEffect(EEZStatusEffectType EffectType)
{
	const int32 Index = FindStatusEffectIndex(EffectType);

	if (Index != INDEX_NONE)
	{
		ActiveStatusEffects.RemoveAt(Index);

		FEZStatusEffectChangedEvent Event;
		Event.EffectType = EffectType;
		Event.bAdded = false;
		Event.TimeRemaining = 0.0f;
		OnStatusEffectChanged.Broadcast(Event);

		UpdateCachedModifiers();
		UpdateCachedSnapshot();
	}
}

void UEZHealthComponent::AddContamination(EEZContaminationType Type, float Amount)
{
	FEZContaminationState& State = GetContaminationStateRef(Type);
	State.Value = FMath::Clamp(State.Value + Amount, 0.0f, 100.0f);
	UpdateContaminationStage(State, Type);
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::ReduceContamination(EEZContaminationType Type, float Amount)
{
	FEZContaminationState& State = GetContaminationStateRef(Type);
	State.Value = FMath::Clamp(State.Value - Amount, 0.0f, 100.0f);
	UpdateContaminationStage(State, Type);
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::UpdateContaminationStage(FEZContaminationState& State, EEZContaminationType Type)
{
	if (Type == EEZContaminationType::Biological)
	{
		State.Stage = FEZHealthStatics::ResolveContaminationStage(
			State.Value,
			Settings.BiologicalStage1Max,
			Settings.BiologicalStage2Max);

		if (State.Stage == EEZContaminationStage::Stage3)
		{
			if (State.LethalTimerRemaining < 0.0f)
			{
				State.LethalTimerRemaining = Settings.BiologicalLethalDuration;
			}
		}
		else
		{
			State.LethalTimerRemaining = -1.0f;
		}
	}
	else
	{
		State.Stage = FEZHealthStatics::ResolveContaminationStage(
			State.Value,
			Settings.RadiationStage1Max,
			Settings.RadiationStage2Max);

		if (State.Stage == EEZContaminationStage::Stage3)
		{
			if (State.LethalTimerRemaining < 0.0f)
			{
				State.LethalTimerRemaining = Settings.RadiationLethalDuration;
			}
		}
		else
		{
			State.LethalTimerRemaining = -1.0f;
		}
	}
}

EEZMedicalItemUseResult UEZHealthComponent::ApplyMedicalAction(const FEZMedicalActionContext& Context)
{
	if (LifeState == EEZLifeState::Dead)
	{
		return EEZMedicalItemUseResult::Failed_InvalidState;
	}

	switch (Context.ActionType)
	{
	case EEZMedicalActionType::Bandage:
		CureBleeding(Context.TargetBodyPart, EEZBleedSeverity::Light);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Tourniquet:
		CureBleeding(Context.TargetBodyPart, EEZBleedSeverity::Heavy);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Splint:
		CureFracture(Context.TargetBodyPart);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Morphine:
		RemoveStatusEffect(EEZStatusEffectType::PainShock);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Antidote:
		RemoveStatusEffect(EEZStatusEffectType::Poison);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Antirad:
		ReduceContamination(EEZContaminationType::Radiation, 50.0f * Context.Potency);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::BurnTreatment:
		ExtinguishBurning(Context.TargetBodyPart);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Water:
		AddHydration(25.0f * Context.Potency);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::Food:
		AddHunger(25.0f * Context.Potency);
		return EEZMedicalItemUseResult::Success;

	case EEZMedicalActionType::StimPack:
		StartAdrenaline(Settings.AdrenalineDuration);
		return EEZMedicalItemUseResult::Success;

	default:
		return EEZMedicalItemUseResult::Failed_NoEffect;
	}
}

void UEZHealthComponent::NotifyHardLanding(float Severity)
{
	const bool bAnyLegFractured = LeftLegState.bFractured || RightLegState.bFractured;
	if (!bAnyLegFractured || Severity <= 0.0f)
	{
		return;
	}

	FEZStatusEffectState PainShock;
	PainShock.EffectType = EEZStatusEffectType::PainShock;
	PainShock.Duration = 30.0f;
	PainShock.TimeRemaining = 30.0f;
	PainShock.Intensity = 1.0f;
	PainShock.SourceBodyPart = LeftLegState.bFractured ? EEZBodyPart::LeftLeg : EEZBodyPart::RightLeg;
	ApplyStatusEffect(PainShock);

	OnScreamRequested.Broadcast();
}

void UEZHealthComponent::NotifyNearExplosion(float Severity)
{
	if (Severity <= 0.0f)
	{
		return;
	}

	FEZStatusEffectState Concussion;
	Concussion.EffectType = EEZStatusEffectType::Concussion;
	Concussion.Duration = FMath::Clamp(Severity, Settings.ConcussionMinDuration, Settings.ConcussionMaxDuration);
	Concussion.TimeRemaining = Concussion.Duration;
	Concussion.Intensity = 1.0f;
	Concussion.SourceBodyPart = EEZBodyPart::Head;
	ApplyStatusEffect(Concussion);
}

void UEZHealthComponent::NotifyElectricExposure(float DurationOverride)
{
	FEZStatusEffectState Electric;
	Electric.EffectType = EEZStatusEffectType::ElectricShock;
	Electric.Duration = DurationOverride > 0.0f ? DurationOverride : Settings.ElectricShockDuration;
	Electric.TimeRemaining = Electric.Duration;
	Electric.Intensity = 1.0f;
	Electric.SourceBodyPart = EEZBodyPart::Torso;
	ApplyStatusEffect(Electric);
}

void UEZHealthComponent::SetMovementStateSnapshot(bool bMoving, bool bSprinting, EEZBodyStance Stance)
{
	bOwnerMoving = bMoving;
	bOwnerSprinting = bSprinting;
	OwnerStance = Stance;
}

void UEZHealthComponent::StartAdrenaline(float Duration)
{
	if (!bAdrenalineActive)
	{
		bAdrenalineActive = true;
		OnAdrenalineStarted.Broadcast();
	}

	AdrenalineTimeRemaining = FMath::Max(AdrenalineTimeRemaining, Duration);

	if (!HasStatusEffect(EEZStatusEffectType::Adrenaline))
	{
		FEZStatusEffectState Adrenaline;
		Adrenaline.EffectType = EEZStatusEffectType::Adrenaline;
		Adrenaline.Duration = Duration;
		Adrenaline.TimeRemaining = Duration;
		Adrenaline.Intensity = 1.0f;
		Adrenaline.SourceBodyPart = EEZBodyPart::Torso;
		ApplyStatusEffect(Adrenaline);
	}

	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::ProcessAdrenaline(float DeltaTime)
{
	if (!bAdrenalineActive)
	{
		return;
	}

	AdrenalineTimeRemaining -= DeltaTime;

	const int32 Index = FindStatusEffectIndex(EEZStatusEffectType::Adrenaline);
	if (Index != INDEX_NONE)
	{
		ActiveStatusEffects[Index].TimeRemaining = AdrenalineTimeRemaining;
	}

	if (AdrenalineTimeRemaining <= 0.0f)
	{
		bAdrenalineActive = false;
		AdrenalineTimeRemaining = 0.0f;
		RemoveStatusEffect(EEZStatusEffectType::Adrenaline);
		OnAdrenalineEnded.Broadcast();

		if (bDeferredPainShock)
		{
			bDeferredPainShock = false;

			FEZStatusEffectState PainShock;
			PainShock.EffectType = EEZStatusEffectType::PainShock;
			PainShock.Duration = Settings.PainShockDuration;
			PainShock.TimeRemaining = Settings.PainShockDuration;
			PainShock.Intensity = 1.0f;
			PainShock.SourceBodyPart = EEZBodyPart::Torso;
			ApplyStatusEffect(PainShock);
		}
	}
}

void UEZHealthComponent::TriggerPainShockIfNeeded(const FEZDamageContext& DamageContext, float DamageApplied, bool bBodyPartDestroyed)
{
	if (!FEZHealthStatics::IsLimb(DamageContext.BodyPart))
	{
		return;
	}

	const bool bShouldTrigger = DamageApplied > 50.0f || bBodyPartDestroyed || DamageContext.bCanCausePainShock;
	if (!bShouldTrigger)
	{
		return;
	}

	if (bAdrenalineActive)
	{
		bDeferredPainShock = true;
		return;
	}

	FEZStatusEffectState PainShock;
	PainShock.EffectType = EEZStatusEffectType::PainShock;
	PainShock.Duration = Settings.PainShockDuration;
	PainShock.TimeRemaining = Settings.PainShockDuration;
	PainShock.Intensity = 1.0f;
	PainShock.SourceBodyPart = DamageContext.BodyPart;
	ApplyStatusEffect(PainShock);
}

void UEZHealthComponent::ProcessBleeding(float DeltaTime)
{
	auto ProcessPart = [this, DeltaTime](EEZBodyPart Part, FEZBodyPartState& State)
		{
			if (State.BleedSeverity == EEZBleedSeverity::None)
			{
				State.AutoClotProgress = 0.0f;
				return;
			}

			float DamagePerSecond = 0.0f;

			if (State.BleedSeverity == EEZBleedSeverity::Light)
			{
				DamagePerSecond = Settings.LightBleedDamagePerSecond;

				if (OwnerStance == EEZBodyStance::Prone && !bOwnerMoving && !bOwnerSprinting)
				{
					State.AutoClotProgress += DeltaTime;
					if (State.AutoClotProgress >= Settings.LightBleedAutoClotTime)
					{
						State.BleedSeverity = EEZBleedSeverity::None;
						State.AutoClotProgress = 0.0f;
						return;
					}
				}
				else
				{
					State.AutoClotProgress = 0.0f;
				}
			}
			else if (State.BleedSeverity == EEZBleedSeverity::Heavy)
			{
				DamagePerSecond = Settings.HeavyBleedDamagePerSecond;
				State.AutoClotProgress = 0.0f;
			}

			ApplyDirectTickDamage(Part, DamagePerSecond * DeltaTime);
		};

	ProcessPart(EEZBodyPart::Head, HeadState);
	ProcessPart(EEZBodyPart::Torso, TorsoState);
	ProcessPart(EEZBodyPart::LeftArm, LeftArmState);
	ProcessPart(EEZBodyPart::RightArm, RightArmState);
	ProcessPart(EEZBodyPart::LeftLeg, LeftLegState);
	ProcessPart(EEZBodyPart::RightLeg, RightLegState);
}

void UEZHealthComponent::ProcessNeeds(float DeltaTime)
{
	float HydrationDecay = Hydration.DecayPerSecond;
	if (BodyTemperature > Settings.HyperthermiaThreshold)
	{
		HydrationDecay *= Settings.HyperthermiaHydrationDecayMultiplier;
	}

	{
		const float OldHydration = Hydration.CurrentValue;
		Hydration.CurrentValue = FEZHealthStatics::ClampNeedValue(Hydration.CurrentValue - HydrationDecay * DeltaTime, Hydration.MaxValue);
		Hydration.bDepleted = Hydration.CurrentValue <= 0.0f;

		if (!FMath::IsNearlyEqual(OldHydration, Hydration.CurrentValue))
		{
			BroadcastNeedChanged(EEZNeedType::Hydration, OldHydration, Hydration.CurrentValue);
		}
	}

	{
		const float OldHunger = Hunger.CurrentValue;
		Hunger.CurrentValue = FEZHealthStatics::ClampNeedValue(Hunger.CurrentValue - Hunger.DecayPerSecond * DeltaTime, Hunger.MaxValue);
		Hunger.bDepleted = Hunger.CurrentValue <= 0.0f;

		if (!FMath::IsNearlyEqual(OldHunger, Hunger.CurrentValue))
		{
			BroadcastNeedChanged(EEZNeedType::Hunger, OldHunger, Hunger.CurrentValue);
		}
	}

	if (Hydration.CurrentValue <= 0.0f)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, Settings.DehydrationTorsoDamagePerSecond * DeltaTime);
	}

	if (Hunger.CurrentValue <= 0.0f)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, Settings.StarvationTorsoDamagePerSecond * DeltaTime);
	}

	if (BodyTemperature < Settings.HypothermiaThreshold)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, (Settings.HypothermiaDamagePerMinute / 60.0f) * DeltaTime);
	}
	else if (BodyTemperature > Settings.HyperthermiaThreshold)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, (Settings.HyperthermiaDamagePerMinute / 60.0f) * DeltaTime);
	}
}

void UEZHealthComponent::ProcessStatusEffects(float DeltaTime)
{
	for (int32 i = ActiveStatusEffects.Num() - 1; i >= 0; --i)
	{
		FEZStatusEffectState& Effect = ActiveStatusEffects[i];
		Effect.TimeRemaining -= DeltaTime;

		switch (Effect.EffectType)
		{
		case EEZStatusEffectType::Poison:
			ApplyDirectTickDamage(Effect.SourceBodyPart, Settings.PoisonDamagePerSecond * Effect.Intensity * DeltaTime);
			break;

		case EEZStatusEffectType::Burning:
			ApplyDirectTickDamage(Effect.SourceBodyPart, Settings.BurnDamagePerSecond * Effect.Intensity * DeltaTime);
			break;

		default:
			break;
		}

		if (Effect.TimeRemaining <= 0.0f)
		{
			const EEZStatusEffectType RemovedType = Effect.EffectType;
			ActiveStatusEffects.RemoveAt(i);

			FEZStatusEffectChangedEvent Event;
			Event.EffectType = RemovedType;
			Event.bAdded = false;
			Event.TimeRemaining = 0.0f;
			OnStatusEffectChanged.Broadcast(Event);
		}
	}
}

void UEZHealthComponent::ProcessContamination(float DeltaTime)
{
	if (BiologicalContamination.Stage == EEZContaminationStage::Stage3)
	{
		BiologicalContamination.LethalTimerRemaining -= DeltaTime;
		if (BiologicalContamination.LethalTimerRemaining <= 0.0f)
		{
			KillOwner();
			return;
		}
	}

	if (RadiationContamination.Stage == EEZContaminationStage::Stage1)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, (Settings.RadiationStage1DamagePerMinute / 60.0f) * DeltaTime);
	}
	else if (RadiationContamination.Stage == EEZContaminationStage::Stage2)
	{
		ApplyDirectTickDamage(EEZBodyPart::Torso, (Settings.RadiationStage2DamagePerMinute / 60.0f) * DeltaTime);

		RadiationContamination.ProcAccumulator += DeltaTime;
		if (RadiationContamination.ProcAccumulator >= 60.0f)
		{
			RadiationContamination.ProcAccumulator = 0.0f;

			if (FMath::FRand() <= Settings.RadiationStage2BleedChancePerMinute)
			{
				const int32 RandomPartIndex = FMath::RandRange(0, 3);
				const EEZBodyPart BleedPart =
					(RandomPartIndex == 0) ? EEZBodyPart::LeftArm :
					(RandomPartIndex == 1) ? EEZBodyPart::RightArm :
					(RandomPartIndex == 2) ? EEZBodyPart::LeftLeg :
					EEZBodyPart::RightLeg;

				FEZBodyPartState& Part = GetBodyPartStateRef(BleedPart);
				if (Part.BleedSeverity == EEZBleedSeverity::None)
				{
					Part.BleedSeverity = EEZBleedSeverity::Light;
					Part.AutoClotProgress = 0.0f;
				}
			}
		}
	}
	else if (RadiationContamination.Stage == EEZContaminationStage::Stage3)
	{
		RadiationContamination.LethalTimerRemaining -= DeltaTime;
		if (RadiationContamination.LethalTimerRemaining <= 0.0f)
		{
			KillOwner();
			return;
		}
	}
}

void UEZHealthComponent::UpdateLifeState()
{
	EEZLifeState NewState = EEZLifeState::Alive;

	if (HeadState.CurrentHealth <= 0.0f || TorsoState.CurrentHealth <= 0.0f)
	{
		NewState = EEZLifeState::Dead;
	}

	if (LifeState != NewState)
	{
		FEZLifeStateChangedEvent Event;
		Event.OldState = LifeState;
		Event.NewState = NewState;

		LifeState = NewState;
		OnLifeStateChanged.Broadcast(Event);

		if (LifeState == EEZLifeState::Dead)
		{
			OnDeath.Broadcast();
		}
	}
}

void UEZHealthComponent::KillOwner()
{
	if (LifeState == EEZLifeState::Dead)
	{
		return;
	}

	HeadState.CurrentHealth = 0.0f;
	TorsoState.CurrentHealth = 0.0f;
	UpdateLifeState();
	UpdateCachedModifiers();
	UpdateCachedSnapshot();
}

void UEZHealthComponent::UpdateCachedModifiers()
{
	FEZHealthModifiers Mod;

	const bool bAnyHeavyBleed =
		HeadState.BleedSeverity == EEZBleedSeverity::Heavy ||
		TorsoState.BleedSeverity == EEZBleedSeverity::Heavy ||
		LeftArmState.BleedSeverity == EEZBleedSeverity::Heavy ||
		RightArmState.BleedSeverity == EEZBleedSeverity::Heavy ||
		LeftLegState.BleedSeverity == EEZBleedSeverity::Heavy ||
		RightLegState.BleedSeverity == EEZBleedSeverity::Heavy;

	if (bAnyHeavyBleed)
	{
		Mod.bBlockStaminaRecovery = true;
	}

	if (LeftLegState.bDestroyed || RightLegState.bDestroyed)
	{
		Mod.MovementSpeedMultiplier *= Settings.LegFractureMoveSpeedMultiplier;
	}

	if (LeftLegState.bFractured || RightLegState.bFractured)
	{
		Mod.MovementSpeedMultiplier *= Settings.LegFractureMoveSpeedMultiplier;
	}

	if (LeftArmState.bFractured || RightArmState.bFractured)
	{
		Mod.WeaponSwayMultiplier *= Settings.ArmFractureWeaponSwayMultiplier;
		Mod.bCanEquipTwoHandedWeapon = false;
		Mod.bPistolOnly = true;
		Mod.ReloadTimeMultiplier *= Settings.ArmFractureReloadMultiplier;
	}

	if (Hydration.CurrentValue <= 0.0f)
	{
		Mod.MaxStaminaMultiplier *= Settings.DehydrationStaminaMultiplier;
		Mod.bCanConsumeDryFood = false;
		Mod.bCanConsumePills = false;
	}

	if (Hunger.CurrentValue <= 0.0f)
	{
		Mod.CarryWeightFlatBonus -= Settings.HungerCarryWeightPenalty;
		Mod.WeaponSwayMultiplier *= Settings.HungerWeaponSwayMultiplier;
	}

	if (BodyTemperature < Settings.HypothermiaThreshold)
	{
		Mod.MovementSpeedMultiplier *= Settings.HypothermiaMoveSpeedMultiplier;
		Mod.ActionDurationMultiplier *= Settings.HypothermiaActionDurationMultiplier;
	}

	if (BodyTemperature > Settings.HyperthermiaThreshold)
	{
		Mod.bCanSprint = false;
	}

	if (BiologicalContamination.Stage == EEZContaminationStage::Stage1)
	{
		Mod.MaxStaminaMultiplier *= Settings.BiologicalStage1PhysicalMultiplier;
		Mod.CarryWeightMultiplier *= Settings.BiologicalStage1PhysicalMultiplier;
		Mod.StaminaRecoveryMultiplier *= Settings.BiologicalStage1PhysicalMultiplier;
	}
	else if (BiologicalContamination.Stage == EEZContaminationStage::Stage2)
	{
		Mod.MaxStaminaMultiplier *= Settings.BiologicalStage2PhysicalMultiplier;
		Mod.CarryWeightMultiplier *= Settings.BiologicalStage2PhysicalMultiplier;
		Mod.StaminaRecoveryMultiplier *= Settings.BiologicalStage2PhysicalMultiplier;
		Mod.bHallucinationsActive = true;
	}
	else if (BiologicalContamination.Stage == EEZContaminationStage::Stage3)
	{
		Mod.MaxStaminaMultiplier *= Settings.BiologicalStage3PhysicalMultiplier;
		Mod.CarryWeightMultiplier *= Settings.BiologicalStage3PhysicalMultiplier;
		Mod.StaminaRecoveryMultiplier *= Settings.BiologicalStage3PhysicalMultiplier;
		Mod.bHallucinationsActive = true;
	}

	if (HasStatusEffect(EEZStatusEffectType::Poison))
	{
		Mod.bPoisonBlurActive = true;
		Mod.bCameraDisorientation = true;
	}

	if (HasStatusEffect(EEZStatusEffectType::PainShock) && !bAdrenalineActive)
	{
		Mod.bCanAim = false;
		Mod.HipFireSpreadMultiplier *= Settings.PainShockHipFireSpreadMultiplier;
		Mod.MaxStaminaMultiplier *= Settings.PainShockMaxStaminaMultiplier;
		Mod.bCanSprint = false;
		Mod.bDoubleVision = true;
	}

	if (HasStatusEffect(EEZStatusEffectType::Concussion))
	{
		Mod.bDeafened = true;
		Mod.LookSensitivityMultiplier *= 0.5f;
		Mod.bHideHUD = true;
	}

	if (HasStatusEffect(EEZStatusEffectType::ElectricShock))
	{
		Mod.WeaponSwayMultiplier *= Settings.ElectricShockWeaponSwayMultiplier;
		Mod.MovementSpeedMultiplier *= Settings.ElectricShockMoveSpeedMultiplier;
		Mod.bStanceChangeLocked = true;
	}

	if (bAdrenalineActive)
	{
		Mod.bFreeSprint = true;
	}

	CachedModifiers = Mod;
	BroadcastModifiersChanged();
}

void UEZHealthComponent::UpdateCachedSnapshot()
{
	CachedSnapshot.LifeState = LifeState;
	CachedSnapshot.Modifiers = CachedModifiers;

	CachedSnapshot.HydrationPercent = GetHydrationPercent();
	CachedSnapshot.HungerPercent = GetHungerPercent();
	CachedSnapshot.BodyTemperature = BodyTemperature;

	CachedSnapshot.HeadHealthPercent = HeadState.GetHealthPercent();
	CachedSnapshot.TorsoHealthPercent = TorsoState.GetHealthPercent();
	CachedSnapshot.LeftArmHealthPercent = LeftArmState.GetHealthPercent();
	CachedSnapshot.RightArmHealthPercent = RightArmState.GetHealthPercent();
	CachedSnapshot.LeftLegHealthPercent = LeftLegState.GetHealthPercent();
	CachedSnapshot.RightLegHealthPercent = RightLegState.GetHealthPercent();

	CachedSnapshot.BiologicalStage = BiologicalContamination.Stage;
	CachedSnapshot.RadiationStage = RadiationContamination.Stage;
}

void UEZHealthComponent::BroadcastModifiersChanged()
{
	FEZModifiersChangedEvent Event;
	Event.Modifiers = CachedModifiers;
	OnModifiersChanged.Broadcast(Event);
}

void UEZHealthComponent::BroadcastNeedChanged(EEZNeedType NeedType, float OldValue, float NewValue)
{
	FEZNeedChangedEvent Event;
	Event.NeedType = NeedType;
	Event.OldValue = OldValue;
	Event.NewValue = NewValue;
	OnNeedChanged.Broadcast(Event);
}