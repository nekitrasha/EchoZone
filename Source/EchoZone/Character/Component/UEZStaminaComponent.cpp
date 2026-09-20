#include "UEZStaminaComponent.h"
#include "UEZCharacterMovementComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UEZStaminaComponent::UEZStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEZStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	MaxStamina = FMath::Max(0.0f, MaxStamina);
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, GetEffectiveMaxStamina());

	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		CachedMovementComponent = Cast<UEZCharacterMovementComponent>(CharacterOwner->GetCharacterMovement());
	}

	if (!CachedMovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEZStaminaComponent: UEZCharacterMovementComponent not found on owner %s"), *GetNameSafe(GetOwner()));
	}

	const bool bShouldBlockSprint = CurrentStamina < MinStaminaToAllowSprint;
	ApplySprintBlock(bShouldBlockSprint);

	BroadcastStaminaChanged();
}

void UEZStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableStaminaSystem)
	{
		return;
	}

	UpdateStamina(DeltaTime);

	if (bEnableStaminaDebug)
	{
		DrawDebug();
	}
}

void UEZStaminaComponent::UpdateStamina(float DeltaTime)
{
	const bool bSprintActive = IsSprintCurrentlyActive();

	if (bSprintActive)
	{
		TimeSinceSprintStopped = 0.0f;

		const float OldStamina = CurrentStamina;
		const float SlopeMultiplier = GetSlopeStaminaMultiplier();
		const float EffectiveMaxStamina = GetEffectiveMaxStamina();
		const float EffectiveDrain = bFreeSprint ? 0.0f : SprintDrainPerSecond * ExternalDrainMultiplier;

		CurrentStamina = FMath::Clamp(
			CurrentStamina - EffectiveDrain * SlopeMultiplier * DeltaTime,
			0.0f,
			EffectiveMaxStamina
		);

		if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
		{
			BroadcastStaminaChanged();
		}

		if (CurrentStamina <= 0.0f && !bFreeSprint)
		{
			ApplySprintBlock(true);
		}
	}
	else
	{
		TimeSinceSprintStopped += DeltaTime;

		if (CanRecoverStamina() && !bExternalRecoveryBlocked)
		{
			const float OldStamina = CurrentStamina;
			const float RecoveryMultiplier = GetSlopeRecoveryMultiplier();
			const float EffectiveMaxStamina = GetEffectiveMaxStamina();

			CurrentStamina = FMath::Clamp(
				CurrentStamina + RecoveryPerSecond * ExternalRecoveryMultiplier * RecoveryMultiplier * DeltaTime,
				0.0f,
				EffectiveMaxStamina
			);

			if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
			{
				BroadcastStaminaChanged();
			}
		}

		if (bStaminaBlockedSprint && CurrentStamina >= MinStaminaToAllowSprint)
		{
			ApplySprintBlock(false);
		}
	}

	bWasSprintActiveLastTick = bSprintActive;
}

void UEZStaminaComponent::ApplySprintBlock(bool bBlocked)
{
	if (bStaminaBlockedSprint == bBlocked)
	{
		return;
	}

	bStaminaBlockedSprint = bBlocked;

	if (CachedMovementComponent)
	{
		CachedMovementComponent->SetMovementBlockFlag(EMovementBlockFlags::Stamina, bBlocked);

		if (bBlocked)
		{
			CachedMovementComponent->SetSprintIntent(false);
		}
	}

	if (bBlocked)
	{
		OnStaminaDepleted.Broadcast();
	}
	else
	{
		OnStaminaRecovered.Broadcast();
	}
}

bool UEZStaminaComponent::CanRecoverStamina() const
{
	return TimeSinceSprintStopped >= RecoveryDelayAfterSprint;
}

bool UEZStaminaComponent::IsSprintCurrentlyActive() const
{
	if (CachedMovementComponent)
	{
		return CachedMovementComponent->IsSprintActive();
	}

	return bManualSprintState;
}

void UEZStaminaComponent::BroadcastStaminaChanged()
{
	OnStaminaChanged.Broadcast(CurrentStamina, GetEffectiveMaxStamina());
}

float UEZStaminaComponent::GetSlopeStaminaMultiplier() const
{
	if (!bUseSlopeStaminaModifier || !CachedMovementComponent || !GetOwner())
	{
		return 1.0f;
	}

	const FFindFloorResult& Floor = CachedMovementComponent->CurrentFloor;
	if (!Floor.bBlockingHit)
	{
		return 1.0f;
	}

	const FVector FloorNormal = Floor.HitResult.ImpactNormal.GetSafeNormal();

	FVector HorizontalVelocity = GetOwner()->GetVelocity();
	HorizontalVelocity.Z = 0.0f;

	if (HorizontalVelocity.IsNearlyZero())
	{
		return 1.0f;
	}

	HorizontalVelocity.Normalize();

	const FVector UphillDirection = FVector::VectorPlaneProject(FVector::UpVector, FloorNormal).GetSafeNormal();
	if (UphillDirection.IsNearlyZero())
	{
		return 1.0f;
	}

	const float MoveDot = FVector::DotProduct(HorizontalVelocity, UphillDirection);
	const float SlopeDot = FMath::Clamp(FVector::DotProduct(FloorNormal, FVector::UpVector), -1.0f, 1.0f);
	const float SlopeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(SlopeDot));
	const float SlopeAlpha = FMath::Clamp(SlopeAngleDeg / MaxSlopeAngleForStaminaEffect, 0.0f, 1.0f);

	if (MoveDot > 0.0f)
	{
		return 1.0f + SlopeAlpha * MoveDot * UphillDrainMultiplierBonus;
	}

	if (MoveDot < 0.0f)
	{
		return 1.0f + SlopeAlpha * MoveDot * DownhillDrainReduction;
	}

	return 1.0f;
}

float UEZStaminaComponent::GetSlopeRecoveryMultiplier() const
{
	if (!bUseSlopeStaminaModifier || !CachedMovementComponent || !GetOwner())
	{
		return 1.0f;
	}

	const FFindFloorResult& Floor = CachedMovementComponent->CurrentFloor;
	if (!Floor.bBlockingHit)
	{
		return 1.0f;
	}

	const FVector FloorNormal = Floor.HitResult.ImpactNormal.GetSafeNormal();

	FVector HorizontalVelocity = GetOwner()->GetVelocity();
	HorizontalVelocity.Z = 0.0f;

	if (HorizontalVelocity.IsNearlyZero())
	{
		return 1.0f;
	}

	HorizontalVelocity.Normalize();

	const FVector UphillDirection = FVector::VectorPlaneProject(FVector::UpVector, FloorNormal).GetSafeNormal();
	if (UphillDirection.IsNearlyZero())
	{
		return 1.0f;
	}

	const float MoveDot = FVector::DotProduct(HorizontalVelocity, UphillDirection);
	const float SlopeDot = FMath::Clamp(FVector::DotProduct(FloorNormal, FVector::UpVector), -1.0f, 1.0f);
	const float SlopeAngleDeg = FMath::RadiansToDegrees(FMath::Acos(SlopeDot));
	const float SlopeAlpha = FMath::Clamp(SlopeAngleDeg / MaxSlopeAngleForStaminaEffect, 0.0f, 1.0f);

	if (MoveDot > 0.0f)
	{
		return FMath::Max(0.1f, 1.0f - SlopeAlpha * MoveDot * UphillRecoveryReduction);
	}

	if (MoveDot < 0.0f)
	{
		return 1.0f + SlopeAlpha * (-MoveDot) * DownhillRecoveryBonus;
	}

	return 1.0f;
}

float UEZStaminaComponent::GetStaminaNormalized() const
{
	const float EffectiveMax = GetEffectiveMaxStamina();
	if (EffectiveMax <= 0.0f)
	{
		return 0.0f;
	}

	return CurrentStamina / EffectiveMax;
}

bool UEZStaminaComponent::IsStaminaEmpty() const
{
	return CurrentStamina <= 0.0f;
}

void UEZStaminaComponent::AddStamina(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.0f, GetEffectiveMaxStamina());

	if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
	{
		BroadcastStaminaChanged();
	}

	if (bStaminaBlockedSprint && CurrentStamina >= MinStaminaToAllowSprint)
	{
		ApplySprintBlock(false);
	}
}

void UEZStaminaComponent::ConsumeStamina(float Amount)
{
	if (Amount <= 0.0f)
	{
		return;
	}

	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, GetEffectiveMaxStamina());

	if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
	{
		BroadcastStaminaChanged();
	}

	if (CurrentStamina <= 0.0f)
	{
		ApplySprintBlock(true);
	}
}

void UEZStaminaComponent::SetCurrentStamina(float NewValue)
{
	const float OldStamina = CurrentStamina;
	CurrentStamina = FMath::Clamp(NewValue, 0.0f, GetEffectiveMaxStamina());

	if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
	{
		BroadcastStaminaChanged();
	}

	const bool bShouldBlockSprint = CurrentStamina < MinStaminaToAllowSprint;
	ApplySprintBlock(bShouldBlockSprint);
}

void UEZStaminaComponent::SetSprinting(bool bNewSprinting)
{
	bManualSprintState = bNewSprinting;
}

void UEZStaminaComponent::SetExternalMaxStaminaMultiplier(float Value)
{
	ExternalMaxStaminaMultiplier = FMath::Max(0.0f, Value);
	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, GetEffectiveMaxStamina());
	BroadcastStaminaChanged();
}

void UEZStaminaComponent::SetExternalDrainMultiplier(float Value)
{
	ExternalDrainMultiplier = FMath::Max(0.0f, Value);
}

void UEZStaminaComponent::SetExternalRecoveryMultiplier(float Value)
{
	ExternalRecoveryMultiplier = FMath::Max(0.0f, Value);
}

void UEZStaminaComponent::SetStaminaRecoveryBlocked(bool bBlocked)
{
	bExternalRecoveryBlocked = bBlocked;
}

void UEZStaminaComponent::SetFreeSprint(bool bEnabled)
{
	bFreeSprint = bEnabled;
}

float UEZStaminaComponent::GetEffectiveMaxStamina() const
{
	return MaxStamina * ExternalMaxStaminaMultiplier;
}

void UEZStaminaComponent::DrawDebug() const
{
	if (!GEngine || !GetOwner())
	{
		return;
	}

	const FString DebugText = FString::Printf(
		TEXT("Stamina: %.1f / %.1f\nNormalized: %.2f\nSprint Blocked: %s\nRecover Delay Timer: %.2f\nSlope Drain Mult: %.2f\nSlope Recover Mult: %.2f\nFreeSprint: %s\nRecoveryBlocked: %s"),
		CurrentStamina,
		GetEffectiveMaxStamina(),
		GetStaminaNormalized(),
		bStaminaBlockedSprint ? TEXT("Yes") : TEXT("No"),
		TimeSinceSprintStopped,
		GetSlopeStaminaMultiplier(),
		GetSlopeRecoveryMultiplier(),
		bFreeSprint ? TEXT("Yes") : TEXT("No"),
		bExternalRecoveryBlocked ? TEXT("Yes") : TEXT("No")
	);

	GEngine->AddOnScreenDebugMessage(
		reinterpret_cast<uint64>(this),
		0.0f,
		FColor::Yellow,
		DebugText
	);
}