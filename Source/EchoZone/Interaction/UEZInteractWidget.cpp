// Fill out your copyright notice in the Description page of Project Settings.


#include "UEZInteractWidget.h"
#include "Components/TextBlock.h"

void UEZInteractWidget::SetInteractText(const FText& NewText)
{
	if (InteractTextBlock)
	{
		InteractTextBlock->SetText(NewText);
	}
}

