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

    CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

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
        CurrentStamina = FMath::Clamp(CurrentStamina - SprintDrainPerSecond * DeltaTime, 0.0f, MaxStamina);

        if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
        {
            BroadcastStaminaChanged();
        }

        if (CurrentStamina <= 0.0f)
        {
            ApplySprintBlock(true);

        }
    }
    else
    {
        TimeSinceSprintStopped += DeltaTime;

        if (CanRecoverStamina())
        {
            const float OldStamina = CurrentStamina;
            CurrentStamina = FMath::Clamp(CurrentStamina + RecoveryPerSecond * DeltaTime, 0.0f, MaxStamina);

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
    return CachedMovementComponent && CachedMovementComponent->IsSprintActive();
}

void UEZStaminaComponent::BroadcastStaminaChanged()
{
    OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

float UEZStaminaComponent::GetStaminaNormalized() const
{
    if (MaxStamina <= 0.0f)
    {
        return 0.0f;
    }

    return CurrentStamina / MaxStamina;
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
    CurrentStamina = FMath::Clamp(CurrentStamina + Amount, 0.0f, MaxStamina);

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
    CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);

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
    CurrentStamina = FMath::Clamp(NewValue, 0.0f, MaxStamina);

    if (!FMath::IsNearlyEqual(OldStamina, CurrentStamina))
    {
        BroadcastStaminaChanged();
    }

    const bool bShouldBlockSprint = CurrentStamina < MinStaminaToAllowSprint;
    ApplySprintBlock(bShouldBlockSprint);

}

void UEZStaminaComponent::DrawDebug() const
{
    if (!GEngine || !GetOwner())
    {
        return;
    }

    const FString DebugText = FString::Printf(
        TEXT("Stamina: %.1f / %.1f\nNormalized: %.2f\nSprint Blocked: %s\nRecover Delay Timer: %.2f"),
        CurrentStamina,
        MaxStamina,
        GetStaminaNormalized(),
        bStaminaBlockedSprint ? TEXT("Yes") : TEXT("No"),
        TimeSinceSprintStopped
    );

    GEngine->AddOnScreenDebugMessage(
        reinterpret_cast<uint64>(this),
        0.0f,
        FColor::Yellow,
        DebugText
    );
}