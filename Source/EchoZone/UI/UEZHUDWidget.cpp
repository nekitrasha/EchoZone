#include "UEZHUDWidget.h"
#include "EchoZone/Character/AEZCharacter.h"
#include "EchoZone/Character/Component/UEZStaminaComponent.h"
#include "EchoZone/Character/Health/Components/UEZHealthComponent.h"
#include "EchoZone/Weapon/AEZWeaponBase.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEZHUDWidget::SetOwnerCharacter(AEZCharacter* InCharacter)
{
	OwnerCharacter = InCharacter;
	RefreshHUD();
}

void UEZHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshHUD();
}

void UEZHUDWidget::RefreshHUD()
{
	if (!OwnerCharacter)
	{
		return;
	}

	UEZHealthComponent* HealthComp = OwnerCharacter->GetHealthComponent();
	UEZStaminaComponent* StaminaComp = OwnerCharacter->GetStaminaComponent();
	AEZWeaponBase* Weapon = OwnerCharacter->GetCurrentWeapon();

	if (HealthComp)
	{
		const float CurrentHP = HealthComp->GetOverallHealthCurrent();
		const float MaxHP = HealthComp->GetOverallHealthMax();
		const float HealthNorm = HealthComp->GetOverallHealthNormalized();

		if (HealthBar)
		{
			HealthBar->SetPercent(HealthNorm);
		}

		if (HealthText)
		{
			HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP %.0f / %.0f"), CurrentHP, MaxHP)));
		}
	}

	if (StaminaComp)
	{
		const float CurrentStamina = StaminaComp->GetCurrentStamina();
		const float MaxStamina = StaminaComp->GetMaxStamina();
		const float StaminaNorm = StaminaComp->GetStaminaNormalized();

		if (StaminaBar)
		{
			StaminaBar->SetPercent(StaminaNorm);
		}

		if (StaminaText)
		{
			StaminaText->SetText(FText::FromString(FString::Printf(TEXT("STM %.0f / %.0f"), CurrentStamina, MaxStamina)));
		}
	}

	if (Weapon)
	{
		const int32 MagAmmo = Weapon->GetCurrentMagazineAmmo();
		const bool bChambered = Weapon->HasRoundChambered();
		const int32 ChamberAmmo = bChambered ? 1 : 0;

		if (AmmoText)
		{
			AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d+%d"), MagAmmo, ChamberAmmo)));
		}

		if (ChamberText)
		{
			ChamberText->SetText(FText::FromString(bChambered ? TEXT("CHAMBER: LOADED") : TEXT("CHAMBER: EMPTY")));
		}

		if (WeaponStateText)
		{
			const FString StateText = Weapon->IsReloading()
				? TEXT("RELOADING")
				: (Weapon->IsAiming() ? TEXT("ADS") : TEXT("READY"));

			WeaponStateText->SetText(FText::FromString(StateText));
		}
	}
	else
	{
		if (AmmoText)
		{
			AmmoText->SetText(FText::FromString(TEXT("--")));
		}

		if (ChamberText)
		{
			ChamberText->SetText(FText::FromString(TEXT("NO WEAPON")));
		}

		if (WeaponStateText)
		{
			WeaponStateText->SetText(FText::FromString(TEXT("UNARMED")));
		}
	}
}