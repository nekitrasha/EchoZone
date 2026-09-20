#include "UEZWeaponPresentationComponent.h"
#include "EchoZone/Weapon/AEZWeaponBase.h"
#include "EchoZone/Weapon/DataAsset/UEZWeaponDataAsset.h"

#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"

UEZWeaponPresentationComponent::UEZWeaponPresentationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEZWeaponPresentationComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* PawnOwner = Cast<APawn>(GetOwner()))
	{
		if (AController* Controller = PawnOwner->GetController())
		{
			LastControlRotation = Controller->GetControlRotation();
		}
	}
}

void UEZWeaponPresentationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentWeapon)
	{
		return;
	}

	UpdateAimAlpha(DeltaTime);
	UpdateVisualRecoil(DeltaTime);
	UpdateWeaponSway(DeltaTime);
	UpdateADSAlignment(DeltaTime);
	ApplyPresentationOffset();
}

void UEZWeaponPresentationComponent::SetCurrentWeapon(AEZWeaponBase* InWeapon)
{
	if (CurrentWeapon == InWeapon)
	{
		return;
	}

	if (CurrentWeapon)
	{
		UnbindFromWeapon(CurrentWeapon);
	}
	
	CurrentWeapon = InWeapon;

	if (CurrentWeapon)
	{
		BindToWeapon(CurrentWeapon);
	}

	ResetPresentation();
}

void UEZWeaponPresentationComponent::SetADSAnchor(USceneComponent* InAnchor)
{
	ADSAnchor = InAnchor;
}

void UEZWeaponPresentationComponent::SetAiming(bool bNewAiming)
{
	bIsAiming = bNewAiming;
}

void UEZWeaponPresentationComponent::NotifyShotFired()
{
	const UEZWeaponDataAsset* WeaponData = GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	TargetVisualLocationOffset += FVector(-FMath::Abs(WeaponData->Recoil.VisualKickLocation), FMath::FRandRange(-0.2f, 0.2f), FMath::FRandRange(-0.15f, 0.15f));
	TargetVisualRotationOffset += FRotator(-FMath::Abs(WeaponData->Recoil.VisualKickRotationPitch), FMath::FRandRange(-WeaponData->Recoil.VisualKickRotationYaw, WeaponData->Recoil.VisualKickRotationYaw), FMath::FRandRange(-0.35f, 0.35f));
}

void UEZWeaponPresentationComponent::UpdateAimAlpha(float DeltaTime)
{
	const UEZWeaponDataAsset* WeaponData = GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	const float Target = bIsAiming ? 1.0f : 0.0f;
	const float Speed = bIsAiming ? (1.0f / FMath::Max(0.01f, WeaponData->ADS.EnterTime)) : (1.0f / FMath::Max(0.01f, WeaponData->ADS.ExitTime));

	CurrentAimAlpha = FMath::FInterpTo(CurrentAimAlpha, Target, DeltaTime, Speed);
}

void UEZWeaponPresentationComponent::UpdateVisualRecoil(float DeltaTime)
{
	const UEZWeaponDataAsset* WeaponData = GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	const float RecoverySpeed = WeaponData->Recoil.VisualRecoverySpeed;

	TargetVisualLocationOffset = FMath::VInterpTo(TargetVisualLocationOffset, FVector::ZeroVector, DeltaTime, RecoverySpeed);
	TargetVisualRotationOffset = FMath::RInterpTo(TargetVisualRotationOffset, FRotator::ZeroRotator, DeltaTime, RecoverySpeed);

	VisualLocationOffset = FMath::VInterpTo(VisualLocationOffset, TargetVisualLocationOffset, DeltaTime, RecoverySpeed * 1.35f);
	VisualRotationOffset = FMath::RInterpTo(VisualRotationOffset, TargetVisualRotationOffset, DeltaTime, RecoverySpeed * 1.35f);
}

void UEZWeaponPresentationComponent::UpdateWeaponSway(float DeltaTime)
{
	const UEZWeaponDataAsset* WeaponData = GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}

	AController* Controller = PawnOwner->GetController();
	if (!Controller)
	{
		return;
	}

	const FRotator CurrentControlRotation = Controller->GetControlRotation();
	const FRotator DeltaRot = (CurrentControlRotation - LastControlRotation).GetNormalized();
	LastControlRotation = CurrentControlRotation;

	const auto& Sway = WeaponData->Sway;

	const FVector TargetLoc(0.0f, FMath::Clamp(-DeltaRot.Yaw * Sway.LocationSwayMultiplier, -Sway.MaxLocationOffset, Sway.MaxLocationOffset), FMath::Clamp(DeltaRot.Pitch * Sway.LocationSwayMultiplier, -Sway.MaxLocationOffset, Sway.MaxLocationOffset));
	const FRotator TargetRot(FMath::Clamp(-DeltaRot.Pitch * Sway.RotationSwayMultiplier, -Sway.MaxRotationOffset, Sway.MaxRotationOffset), FMath::Clamp(-DeltaRot.Yaw * Sway.RotationSwayMultiplier, -Sway.MaxRotationOffset, Sway.MaxRotationOffset), 0.0f);

	SwayLocationOffset = FMath::VInterpTo(SwayLocationOffset, TargetLoc, DeltaTime, Sway.InterpSpeed);
	SwayRotationOffset = FMath::RInterpTo(SwayRotationOffset, TargetRot, DeltaTime, Sway.InterpSpeed);
}

void UEZWeaponPresentationComponent::UpdateADSAlignment(float DeltaTime)
{
	const UEZWeaponDataAsset* WeaponData = GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	FVector TargetLoc = FVector::ZeroVector;
	FRotator TargetRot = FRotator::ZeroRotator;

	if (ADSAnchor && bIsAiming)
	{
		TargetLoc = WeaponData->ADS.ADSOffset;
		TargetRot = WeaponData->ADS.ADSRotationOffset;
	}

	const float Speed = bIsAiming ? (1.0f / FMath::Max(0.01f, WeaponData->ADS.EnterTime)) : (1.0f / FMath::Max(0.01f, WeaponData->ADS.ExitTime));
	ADSLocationOffset = FMath::VInterpTo(ADSLocationOffset, TargetLoc, DeltaTime, Speed);
	ADSRotationOffset = FMath::RInterpTo(ADSRotationOffset, TargetRot, DeltaTime, Speed);
}

void UEZWeaponPresentationComponent::ApplyPresentationOffset()
{
	if (!CurrentWeapon)
	{
		return;
	}

	USceneComponent* VisualRoot = CurrentWeapon->GetWeaponVisualRoot();
	if (!VisualRoot)
	{
		return;
	}

	const FVector FinalLocation = ADSLocationOffset + SwayLocationOffset + VisualLocationOffset;
	const FRotator FinalRotation = UKismetMathLibrary::ComposeRotators(ADSRotationOffset, UKismetMathLibrary::ComposeRotators(SwayRotationOffset, VisualRotationOffset));

	VisualRoot->SetRelativeLocation(FinalLocation);
	VisualRoot->SetRelativeRotation(FinalRotation);
}

void UEZWeaponPresentationComponent::ResetPresentation()
{
	CurrentAimAlpha = 0.0f;
	VisualLocationOffset = FVector::ZeroVector;
	VisualRotationOffset = FRotator::ZeroRotator;
	TargetVisualLocationOffset = FVector::ZeroVector;
	TargetVisualRotationOffset = FRotator::ZeroRotator;
	SwayLocationOffset = FVector::ZeroVector;
	SwayRotationOffset = FRotator::ZeroRotator;
	ADSLocationOffset = FVector::ZeroVector;
	ADSRotationOffset = FRotator::ZeroRotator;
}

const UEZWeaponDataAsset* UEZWeaponPresentationComponent::GetWeaponData() const
{
	return CurrentWeapon ? CurrentWeapon->GetWeaponData() : nullptr;
}

void UEZWeaponPresentationComponent::BindToWeapon(AEZWeaponBase* InWeapon)
{
	if (!InWeapon)
	{
		return;
	}

	InWeapon->OnShotFiredEvent.AddDynamic(this, &UEZWeaponPresentationComponent::NotifyShotFired);
}

void UEZWeaponPresentationComponent::UnbindFromWeapon(AEZWeaponBase* InWeapon)
{
	if (InWeapon == nullptr)
	{
		return;
	}

	InWeapon->OnShotFiredEvent.RemoveDynamic(this, &UEZWeaponPresentationComponent::NotifyShotFired);
}