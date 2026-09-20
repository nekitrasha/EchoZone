// Fill out your copyright notice in the Description page of Project Settings.


#include "UEZInteractComponent.h"
#include "UEZInteractableInterface.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

// Sets default values for this component's properties
UEZInteractComponent::UEZInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEZInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UEZInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateInteractable();
}

void UEZInteractComponent::UpdateInteractable()
{
	AActor* NewInteractable = FindInterectableActor();

	if (CurrentInteractable != NewInteractable)
	{
		CurrentInteractable = NewInteractable;
		OnInteractable.Broadcast(CurrentInteractable);
	}
}

AActor* UEZInteractComponent::FindInterectableActor() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return nullptr;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	OwnerPawn->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * InteractionDistance;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UEZInteractionTrace), false, GetOwner());

	FHitResult HitResult;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel, QueryParams);

	if (bEnableInteractionDebug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 0.03f, 0, 1.0f);
		
		if (bHit)
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 8.0f, 12, FColor::Yellow, false, 0.03f);
		}
	}

	if (!bHit || !HitResult.GetActor())
	{
		return nullptr;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor->GetClass()->ImplementsInterface(UEZInteractableInterface::StaticClass()))
	{
		return nullptr;
	}

	const bool bCanInteract = IEZInteractableInterface::Execute_CanInteract(HitActor, GetOwner());
	return bCanInteract ? HitActor : nullptr;
}

void UEZInteractComponent::TryInteract()
{
	if (bInteractionBlocked)
	{
		return;
	}
	if (!CurrentInteractable)
	{
		return;
	}
	if (!CurrentInteractable->GetClass()->ImplementsInterface(UEZInteractableInterface::StaticClass()))
	{
		return;
	}
	if (!IEZInteractableInterface::Execute_CanInteract(CurrentInteractable, GetOwner()))
	{
		return;
	}

	IEZInteractableInterface::Execute_Interact(CurrentInteractable, GetOwner());
}

FText UEZInteractComponent::GetCurrentInteractText() const
{
	if (!CurrentInteractable)
	{
		return FText::GetEmpty();
	}
	if (!CurrentInteractable->GetClass()->ImplementsInterface(UEZInteractableInterface::StaticClass()))
	{
		return FText::GetEmpty();
	}

	return IEZInteractableInterface::Execute_GetInteractText(CurrentInteractable);
}