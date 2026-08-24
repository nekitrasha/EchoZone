// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UEZInteractableInterface.h"
#include "AEZMedkitPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOZONE_API AEZMedkitPickup : public AActor, public IEZInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZMedkitPickup();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	float HealthAmount = 35.0f;

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractText_Implementation() const override;
};
