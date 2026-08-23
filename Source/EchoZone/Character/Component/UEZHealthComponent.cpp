// Fill out your copyright notice in the Description page of Project Settings.


#include "UEZHealthComponent.h"

// Sets default values for this component's properties
UEZHealthComponent::UEZHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UEZHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UEZHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.0f || IsDead())
	{
		return;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);

	if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth))
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
}

void UEZHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.0f || IsDead())
	{
		return;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - Amount, 0.0f, MaxHealth);

	if (!FMath::IsNearlyEqual(OldHealth, CurrentHealth))
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

		if (CurrentHealth <= 0.0f)
		{
			OnDeath.Broadcast();
		}
	}
}