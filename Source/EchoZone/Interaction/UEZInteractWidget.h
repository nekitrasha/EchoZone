#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UEZInteractWidget.generated.h"

class UTextBlock;

UCLASS()
class ECHOZONE_API UEZInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractText(const FText& NewText);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InteractTextBlock;
};
