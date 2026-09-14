#include "AEZWeaponBase.h"
#include "AEZProjectile.h"
#include "DataAsset/UEZAmmoDataAsset.h"
#include "DataAsset/UEZWeaponDataAsset.h"
#include "DataAsset/UEZMagazineDataAsset.h"
#include "EchoZone/Character/Component/UEZCharacterMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AEZWeaponBase::AEZWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponVisualRoot"));
	WeaponVisualRoot->SetupAttachment(RootScene);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponVisualRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
	MuzzlePoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

void AEZWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeFromData();
}

void AEZWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSpread(DeltaSeconds);
	UpdateRecoil(DeltaSeconds);
	UpdateADS(DeltaSeconds);
}

EEZWeaponType AEZWeaponBase::GetWeaponType() const
{
	return WeaponData ? WeaponData->WeaponType : EEZWeaponType::None;
}

EEZAmmoCaliber AEZWeaponBase::GetWeaponCaliber() const
{
	return WeaponData ? WeaponData->Caliber : EEZAmmoCaliber::None;
}

EEZReloadType AEZWeaponBase::GetReloadType() const
{
	return WeaponData ? WeaponData->ReloadType : EEZReloadType::None;
}

EEZWeaponFeedType AEZWeaponBase::GetFeedType() const
{
	return WeaponData ? WeaponData->FeedType : EEZWeaponFeedType::None;
}

bool AEZWeaponBase::IsMagazineCompatible(const UEZMagazineDataAsset* MagazineData) const
{
	return WeaponData && MagazineData && WeaponData->Caliber == MagazineData->Caliber;
}

bool AEZWeaponBase::IsAmmoCompatible(const UEZAmmoDataAsset* AmmoData) const
{
	return WeaponData && AmmoData && WeaponData->Caliber == AmmoData->Caliber;
}

int32 AEZWeaponBase::GetCurrentMagazineAmmo() const
{
	return InsertedMagazine.IsValid() ? InsertedMagazine.CurrentAmmo : 0;
}

int32 AEZWeaponBase::GetAmmoReadyToFire() const
{
	return GetCurrentMagazineAmmo() + (bRoundChambered ? 1 : 0);
}

int32 AEZWeaponBase::GetMagazineCapacity() const
{
	return InsertedMagazine.GetCapacity();
}

bool AEZWeaponBase::HasInsertedMagazine() const
{
	return InsertedMagazine.IsValid();
}

bool AEZWeaponBase::HasSpareMagazine() const
{
	return FindBestMagazineIndex() != INDEX_NONE;
}

void AEZWeaponBase::SwitchFireMode()
{
	if (!WeaponData || WeaponData->SupportedFireModes.Num() == 0)
	{
		return;
	}

	const int32 CurrentIndex = WeaponData->SupportedFireModes.IndexOfByKey(CurrentFireMode);
	const int32 NextInddex = (CurrentIndex == INDEX_NONE) ? 0 : (CurrentIndex + 1) % WeaponData->SupportedFireModes.Num();

	CurrentFireMode = WeaponData->SupportedFireModes[NextInddex];
	BurstShotRemaining = 0;

	OnFireModeChangedEvent.Broadcast(CurrentFireMode);
	BP_OnFireModeChanged(CurrentFireMode);
}

void AEZWeaponBase::InitializeFromData()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponData is null on %s"), *GetName());
		return;
	}

	CurrentAmmoData = WeaponData->DefaultAmmo;

	if (WeaponData->SupportedFireModes.Num() > 0)
	{
		CurrentFireMode = WeaponData->SupportedFireModes.Contains(WeaponData->DefaultFireMode) ? WeaponData->DefaultFireMode : WeaponData->SupportedFireModes[0];
	}
	else
	{
		CurrentFireMode = WeaponData->DefaultFireMode;
	}

	InsertedMagazine = FEZMagazineInstance();
	if (WeaponData->DefaultMagazine)
	{
		InsertedMagazine.MagazineData = WeaponData->DefaultMagazine;
		InsertedMagazine.CurrentAmmo = WeaponData->DefaultMagazine->Capacity;
	}

	bRoundChambered = false;
	TryChamberedNextRound();

	CurrentSpreadAngle = WeaponData->Spread.BaseSpreadAngle;
	CurrentRecoilMultiplier = 1.0f;
	CurrentAimAlpha = 0.0f;

	bIsReloading = false;
	bTriggerHeld = false;
	bWasEmptyClick = false;
	BurstShotRemaining = 0;
	bTacticalReloadInProgress = false;
}

bool AEZWeaponBase::CanFire() const
{
	if (!WeaponData || !CurrentAmmoData)
	{
		return false;
	}

	if (CurrentFireMode == EEZFireMode::Safe)
	{
		return false;
	}

	const bool bHasProjectileClass = WeaponData->ProjectileClassOverride || ProjectileClass;
	if (!bHasProjectileClass)
	{
		return false;
	}

	return !bIsReloading && bRoundChambered && (WeaponData->bCanFireWhileSprinting || !IsOwnerSprinting());
}

bool AEZWeaponBase::CanReload() const
{
	if (bIsReloading)
	{
		return false;
	}

	if (!HasSpareMagazine())
	{
		return false;
	}

	if (!InsertedMagazine.IsValid())
	{
		return true;
	}

	if (InsertedMagazine.CurrentAmmo <= 0 && !bRoundChambered)
	{
		return true;
	}

	return WeaponData ? WeaponData->bSupportsTacticalReload : true;
}

void AEZWeaponBase::StartFire()
{
	bTriggerHeld = true;

	if (!CanShootCurrentMode())
	{
		return;
	}

	if (!CanFire)
	{
		if (!HasRoundChambered())
		{
			PlayEmptyClick();
		}
		return;
	}

	switch (CurrentFireMode)
	{
	case EEZFireMode::SemiAuto:
		HandleFireModeShot();
		break;

	case EEZFireMode::Burst:
		BurstShotRemaining = WeaponData ? FMath::Max(1, WeaponData->BurstCount) : 3;
		HandleBurstProgress();
		break;

	case EEZFireMode::FullAuto:
		HandleFireModeShot();
		if (bTriggerHeld)
		{
			GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &AEZWeaponBase::HandleAutoFire, GetSecondsPerShot(), true);
		}
		break;

	case EEZFireMode::Safe:
	default:
		break;
	}
}

void AEZWeaponBase::StopFire()
{
	bTriggerHeld = false;
	BurstShotRemaining = 0;
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
}

void AEZWeaponBase::HandleAutoFire()
{
	if (!bTriggerHeld || CurrentFireMode != EEZFireMode::FullAuto || !CanFire())
	{
		if (!HasRoundChambered())
		{
			PlayEmptyClick();
		}

		StopFire();
		return;
	}

	FireShot();
}

void AEZWeaponBase::HandleFireModeShot()
{
	if (!CanFire())
	{
		if (!HasRoundChambered())
		{
			PlayEmptyClick();
		}

		StopFire();
		return;
	}

	FireShot();
	if (CurrentFireMode == EEZFireMode::SemiAuto)
	{
		StopFire();
	}
}

void AEZWeaponBase::HandleBurstProgress()
{
	if (BurstShotRemaining <= 0)
	{
		StopFire();
		return;
	}

	if (!CanFire())
	{
		if (!HasRoundChambered())
		{
			PlayEmptyClick();
		}

		StopFire();
		return;
	}

	FireShot();
	--BurstShotRemaining;

	if (BurstShotRemaining > 0)
	{
		GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &AEZWeaponBase::HandleBurstProgress, GetSecondsPerShot(), false);
	}
	else
	{
		StopFire();
	}
}

void AEZWeaponBase::FireShot()
{
	if (!CanFire() || !GetWorld() || !MuzzlePoint || !WeaponData)
	{
		return;
	}

	const FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	const FVector ShotDirection = GetShotDirection(SpawnLocation);
	const FRotator SpawnRotation = ShotDirection.Rotation();

	TSubclassOf<AEZProjectile> FinalProjectileClass = ProjectileClass;
	if (WeaponData->ProjectileClassOverride)
	{
		FinalProjectileClass = WeaponData->ProjectileClassOverride;
	}

	if (!FinalProjectileClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEZProjectile* Projectile = GetWorld()->SpawnActor<AEZProjectile>(FinalProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile && CurrentAmmoData)
	{
		Projectile->InitProjectile(CurrentAmmoData, ShotDirection);
	}

	if (bDrawDebugShot)
	{
		DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShotDirection * 5000.0f, FColor::Cyan, false, 1.0f, 0, 1.0f);
	}

	ConsumeRound();

	CurrentSpreadAngle = FMath::Clamp(CurrentSpreadAngle + WeaponData->Spread.SpreadPerShot, GetEffectiveBaseSpread(), WeaponData->Spread.MaxSpreadAngle);

	CurrentRecoilMultiplier = FMath::Clamp(CurrentRecoilMultiplier + WeaponData->Recoil.RecoilKickPerShot, 1.0f, WeaponData->Recoil.MaxRecoilMultiplier);

	ApplyRecoil();

	OnShotFiredEvent.Broadcast();
	BP_OnShotFired();
}