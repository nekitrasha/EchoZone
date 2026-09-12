// Fill out your copyright notice in the Description page of Project Settings.


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

// Sets default values
AEZWeaponBase::AEZWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponVisualRoot"));
	WeaponVisualRoot->SetupAttachment(RootScene);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponVisualRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePont"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
	MuzzlePoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
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
	UpdateVisualOffset(DeltaSeconds);
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


}


void AEZWeaponBase::InitializeFromData()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponData is null on %s"), *GetName());
		return;
	}

	CurrentAmmoData = WeaponData->DefaultAmmo;
	
	CurrentFireMode = EEZFireMode::SemiAuto;

	if (WeaponData->SupportedFireModes.Num() > 0)
	{
		CurrentFireMode = WeaponData->SupportedFireModes.Contains(WeaponData->DefaultFireMode) ? WeaponData->DefaultFireMode : WeaponData->SupportedFireModes[0];
	}
	else
	{
		CurrentFireMode = WeaponData->DefaultFireMode;
	}

	if (WeaponData->DefaultMagazine)
	{
		InsertedMagazine.MagazineData = WeaponData->DefaultMagazine;
		InsertedMagazine.CurrentAmmo = WeaponData->DefaultMagazine->Capacity;
	}

	bRoundChambered = false;
	TryChamberNextRound();

	CurrentSpreadAngle = WeaponData->Spread.BaseSpreadAngle;
	CurrentRecoilMultiplier = 1.0f;
	CurrentAimAlpha = 0.0f;
}

bool AEZWeaponBase::CanFire() const
{
	if (!WeaponData || !CurrentAmmoData)
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

	const int32 Capacity = InsertedMagazine.GetCapacity();
	return InsertedMagazine.CurrentAmmo < Capacity;
}

void AEZWeaponBase::StartFire()
{
	bTriggerHeld = true;

	if (!CanFire())
	{
		return;
	}

	FireShot();

	if (CurrentFireMode == EEZFireMode::FullAuto)
	{
		const float TimeBetweenShot = 60.0f / FMath::Max(WeaponData ? WeaponData->FireRateRPM : 600.0f, 1.0f);

		GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &AEZWeaponBase::HandleAutoFire, TimeBetweenShot, true, TimeBetweenShot);
	}	
}

void AEZWeaponBase::StopFire()
{
	bTriggerHeld = false;
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
}

void AEZWeaponBase::HandleAutoFire()
{
	if (!bTriggerHeld || !CanFire())
	{
		StopFire();
		return;
	}
	FireShot();
}

void AEZWeaponBase::FireShot()
{
	if (!CanFire() || !GetWorld() || !MuzzlePoint)
	{
		return;
	}

	const FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	const FVector ShotDirection = GetShotDirection(SpawnLocation);
	const FRotator SpawnRotation = ShotDirection.Rotation();

	TSubclassOf<AEZProjectile> FinalProjectileClass = ProjectileClass;
	if (WeaponData && WeaponData->ProjectileClassOverride)
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

	AEZProjectile* Projectile = GetWorld()->SpawnActor<AEZProjectile>(
		FinalProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	if (Projectile && CurrentAmmoData)
	{
		Projectile->InitProjectile(CurrentAmmoData, ShotDirection);
	}

	ConsumeRound();

	CurrentSpreadAngle = FMath::Clamp(
		CurrentSpreadAngle + WeaponData->Spread.SpreadPerShot,
		GetEffectiveBaseSpread(),
		WeaponData->Spread.MaxSpreadAngle
	);

	CurrentRecoilMultiplier = FMath::Clamp(
		CurrentRecoilMultiplier + WeaponData->Recoil.RecoilKickPerShot,
		1.0f,
		WeaponData->Recoil.MaxRecoilMultiplier
	);

	ApplyRecoil();
}

void AEZWeaponBase::ConsumeRound()
{
	if (!bRoundChambered)
	{
		return;
	}

	bRoundChambered = false;
	TryChamberNextRound();
}

bool AEZWeaponBase::TryChamberNextRound()
{
	if (!InsertedMagazine.IsValid())
	{
		return false;
	}

	if (InsertedMagazine.CurrentAmmo <= 0)
	{
		return false;
	}

	InsertedMagazine.CurrentAmmo--;
	bRoundChambered = true;
	return true;
}

int32 AEZWeaponBase::FindBestMagazineIndex() const
{
	if (!WeaponData)
	{
		return INDEX_NONE;
	}

	int32 BestIndex = INDEX_NONE;
	int32 BestAmmo = -1;

	for (int32 i = 0; i < MagazineInventory.Num(); i++)
	{
		const FEZMagazineInstance& Candidate = MagazineInventory[i];
		if (!Candidate.IsValid() || Candidate.CurrentAmmo <= 0)
		{
			continue;
		}

		if (Candidate.MagazineData->Caliber != WeaponData->Caliber)
		{
			continue;
		}

		if (Candidate.CurrentAmmo > BestAmmo)
		{
			BestAmmo = Candidate.CurrentAmmo;
			BestIndex = i;
		}
	}

	return BestIndex;
}

void AEZWeaponBase::InsertMagazine(const FEZMagazineInstance& NewMagazine)
{
	InsertedMagazine = NewMagazine;
}

FEZMagazineInstance AEZWeaponBase::RemoveInsertedMagazine()
{
	FEZMagazineInstance OldMagazine = InsertedMagazine;
	InsertedMagazine = FEZMagazineInstance();
	return OldMagazine;
}

void AEZWeaponBase::Reload()
{
	if (!CanReload())
	{
		return;
	}

	bIsReloading = true;
	StopFire();

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AEZWeaponBase::FinishReload, GetReloadDuration(), false);
}

void AEZWeaponBase::FinishReload()
{
	if (InsertedMagazine.IsValid())
	{
		MagazineInventory.Add(InsertedMagazine);
		InsertedMagazine = FEZMagazineInstance();
	}

	const int32 NewMagIndex = FindBestMagazineIndex();
	if (NewMagIndex != INDEX_NONE)
	{
		InsertedMagazine = MagazineInventory[NewMagIndex];
		MagazineInventory.RemoveAt(NewMagIndex);
	}

	if (!bRoundChambered)
	{
		TryChamberNextRound();
	}

	bIsReloading = false;
}

void AEZWeaponBase::StartAim()
{
	bIsAiming = true;
}

void AEZWeaponBase::StopAim()
{
	bIsAiming = false;
}

float AEZWeaponBase::GetAimFOV() const
{
	return WeaponData ? WeaponData->ADS.AimFOV : 70.0f;
}

void AEZWeaponBase::UpdateSpread(float DeltaSeconds)
{
	if (!WeaponData)
	{
		return;
	}

	const float TargetSpread = GetEffectiveBaseSpread();
	CurrentSpreadAngle = FMath::FInterpTo(CurrentSpreadAngle, TargetSpread, DeltaSeconds, WeaponData->Spread.RecoverySpeed);
}

void AEZWeaponBase::UpdateRecoil(float DeltaSeconds)
{
	if (!WeaponData)
	{
		return;
	}
	CurrentRecoilMultiplier = FMath::FInterpTo(CurrentRecoilMultiplier, 1.0f, DeltaSeconds, WeaponData->Recoil.RecoverySpeed);
}

void AEZWeaponBase::UpdateADS(float DeltaSeconds)
{
	if (!WeaponData)
	{
		return;
	}

	const float Target = bIsAiming ? 1.0f : 0.0f;
	const float Speed = bIsAiming ? (WeaponData->ADS.EnterTime > 0.0f ? 1.0f / WeaponData->ADS.EnterTime : 999.0f) : (WeaponData->ADS.ExitTime > 0.0f ? 1.0f / WeaponData->ADS.ExitTime : 999.0f);

	CurrentAimAlpha = FMath::FInterpTo(CurrentAimAlpha, Target, DeltaSeconds, Speed);
}

void AEZWeaponBase::UpdateVisualOffset(float DeltaSeconds)
{
	if (!WeaponVisualRoot || !WeaponData)
	{
		return;
	}

	const FVector TargetLocation = FMath::Lerp(FVector::ZeroVector, WeaponData->ADS.ADSOffset, CurrentAimAlpha);
	const FRotator TargetRotation = FMath::Lerp(FRotator::ZeroRotator, WeaponData->ADS.ADSRotationOffset, CurrentAimAlpha);

	const FVector NewLocation = FMath::VInterpTo(WeaponVisualRoot->GetRelativeLocation(), TargetLocation, DeltaSeconds, 12.0f);
	const FRotator NewRotation = FMath::RInterpTo(WeaponVisualRoot->GetRelativeRotation(), TargetRotation, DeltaSeconds, 12.0f);

	WeaponVisualRoot->SetRelativeLocation(NewLocation);
	WeaponVisualRoot->SetRelativeRotation(NewRotation);
}

FVector AEZWeaponBase::GetCameraAimPoint() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !GetWorld())
	{
		return MuzzlePoint ? MuzzlePoint->GetComponentLocation() + MuzzlePoint->GetForwardVector() * 50000.0f : FVector::ZeroVector;
	}

	const AController* Controller = OwnerPawn->GetController();
	if (!Controller)
	{
		return MuzzlePoint ? MuzzlePoint->GetComponentLocation() + MuzzlePoint->GetForwardVector() * 50000.0f : FVector::ZeroVector;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * 50000.0f;

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return Hit.ImpactPoint;
	}

	return TraceEnd;
}

FVector AEZWeaponBase::GetShotDirection(const FVector& FromLocation) const
{
	const FVector AimPoint = GetCameraAimPoint();
	FVector Direction = (AimPoint - FromLocation).GetSafeNormal();

	const float HalfRad = FMath::DegreesToRadians(CurrentSpreadAngle);
	Direction = FMath::VRandCone(Direction, HalfRad, HalfRad).GetSafeNormal();

	return Direction;
}

void AEZWeaponBase::ApplyRecoil()
{
	if (!WeaponData)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	const float PitchRecoil = GetEffectiveVerticalRecoil() * CurrentRecoilMultiplier;
	const float YawRecoil = FMath::FRandRange(-GetEffectiveHorizontalRecoil(), GetEffectiveHorizontalRecoil()) * CurrentRecoilMultiplier;

	OwnerPawn->AddControllerPitchInput(-PitchRecoil);
	OwnerPawn->AddControllerYawInput(YawRecoil);
}

float AEZWeaponBase::GetErgonomicsNormalized() const
{
	return WeaponData ? FMath::Clamp(WeaponData->Ergonomics / 100.0f, 0.0f, 1.0f) : 0.5f;
}

float AEZWeaponBase::GetMovementSpreadMultiplier() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter || !WeaponData)
	{
		return 1.0f;
	}

	const UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return 1.0f;
	}

	float Multiplier = 1.0f;

	if (MoveComp->IsFalling())
	{
		Multiplier *= WeaponData->Spread.InAirMultiplier;
	}

	if (OwnerCharacter->bIsCrouched)
	{
		Multiplier *= WeaponData->Spread.CrouchMultiplier;
	}

	const FVector HorizontalVelocity = FVector(OwnerCharacter->GetVelocity().X, OwnerCharacter->GetVelocity().Y, 0.0f);
	if (!HorizontalVelocity.IsNearlyZero())
	{
		Multiplier *= WeaponData->Spread.MovingMultiplier;
	}

	if (bIsAiming)
	{
		Multiplier *= WeaponData->Spread.ADSMultiplier;
	}

	return Multiplier;
}

float AEZWeaponBase::GetEffectiveBaseSpread() const
{
	if (!WeaponData)
	{
		return 1.0f;
	}

	const float Ergo = GetErgonomicsNormalized();
	const float ErgoSpreadMultiplier = FMath::Lerp(1.25f, 0.8f, Ergo);

	return WeaponData->Spread.BaseSpreadAngle * ErgoSpreadMultiplier * GetMovementSpreadMultiplier();
}

float AEZWeaponBase::GetEffectiveVerticalRecoil() const
{
	if (!WeaponData)
	{
		return 1.0f;
	}

	const float Ergo = GetErgonomicsNormalized();
	return WeaponData->Recoil.VerticalRecoil * FMath::Lerp(1.25, 0.75, Ergo);
}

float AEZWeaponBase::GetEffectiveHorizontalRecoil() const
{
	if (!WeaponData)
	{
		return 1.0f;
	}

	const float Ergo = GetErgonomicsNormalized();
	return WeaponData->Recoil.HorizontalRecoil * FMath::Lerp(1.25f, 0.75, Ergo);
}

bool AEZWeaponBase::IsOwnerSprinting() const
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return false;
	}

	const UEZCharacterMovementComponent* EZMoveComp = Cast<UEZCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
	if (!EZMoveComp)
	{
		return false;
	}

	return EZMoveComp->IsSprintActive();
}

int32 AEZWeaponBase::GetMagazineCapacity() const
{
	return InsertedMagazine.GetCapacity();
}

float AEZWeaponBase::GetReloadDuration() const
{
	if (!InsertedMagazine.IsValid() || !InsertedMagazine.MagazineData)
	{
		return 2.2f;
	}

	const bool bEmptyGun = !bRoundChambered && InsertedMagazine.CurrentAmmo <= 0;
	return bEmptyGun ? InsertedMagazine.MagazineData->EmpryReloadTime : InsertedMagazine.MagazineData->ReloadTime;
}