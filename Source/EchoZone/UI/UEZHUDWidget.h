#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UEZHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AEZCharacter;

UCLASS()
class ECHOZONE_API UEZHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetOwnerCharacter(AEZCharacter* InCharacter);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void RefreshHUD();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthBar;
	
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* StaminaBar;
	
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* StaminaText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* ChamberText;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* WeaponStateText;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	TObjectPtr<AEZCharacter> OwnerCharacter;
};
