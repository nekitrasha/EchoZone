// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UEZInteractableInterface.h"
#include "AEZTestInteractActor.generated.h"

UCLASS()
class ECHOZONE_API AEZTestInteractActor : public AActor, public UEZInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZTestInteractActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractText = FText::FromString(TEXT("Use"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bCanCurrentlyInteact = true;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	bool bWasUsed = false;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractText_Implementation() const override;
};
