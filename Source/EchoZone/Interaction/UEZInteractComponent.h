// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEZInteractComponent.generated.h"

class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractable, AActor*, NewInteractable);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOZONE_API UEZInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEZInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractable OnInteractable;
		
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bEnableInteractionDebug = false;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	AActor* CurrentInteractable = nullptr;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction")
	bool bInteractionBlocked = false;

protected:
	void UpdateInteractable();
	AActor* FindInterectableActor() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetCurrentInteractable() const { return CurrentInteractable; }

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	bool HasInteractable() const { return CurrentInteractable != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	FText GetCurrentInteractText() const;

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetInteractionBlocked(bool bBlocked) { bInteractionBlocked = bBlocked; }
};
