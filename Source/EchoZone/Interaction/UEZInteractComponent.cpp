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

	}
}