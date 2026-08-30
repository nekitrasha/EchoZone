// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZWeaponBase.h"
#include "AEZProjectile.h"
#include "EchoZone/Weapon/DataAsset/UEZAmmoDataAsset.h"
#include "EchoZone/Weapon/DataAsset/UEZWeaponDataAsset.h"
#include "EchoZone/Weapon/DataAsset/UEZMagazineDataAsset.h"
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

void AEZWeaponBase::InitializeFromData()
{
	if (!WeaponData)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponData is null on %s"), *GetName());
		return;
	}

	CurrentAmmoData = WeaponData->DefaultAmmo;
	CurrentMagazineData = WeaponData->DefaultMagazine;
	CurrentFireMode = WeaponData->SupportedFireModes.Num() > 0 ? WeaponData->DefaultFireMode : EEZFireMode::SemiAuto;
	CurrentMagazineAmmo = GetMagazineCapacity();
	bRoundChambered = CurrentMagazineAmmo > 0;
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

	const bool bHasAmmo = bRoundChambered || CurrentMagazineAmmo > 0;

	return !bIsReloading && bHasAmmo && (WeaponData->bCanFireWhileSprinting || !IsOwnerSprinting());
}

bool AEZWeaponBase::CanReload() const
{
	return !bIsReloading && CurrentMagazineData && CurrentMagazineAmmo < GetMagazineCapacity() && ReserveAmmo > 0;
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

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEZProjectile* Projectile = GetWorld()->SpawnActor<AEZProjectile>(FinalProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile && CurrentAmmoData)
	{
		Projectile->InitProjectile(CurrentAmmoData, ShotDirection);
	}

	ConsumeRound();

	CurrentSpreadAngle = FMath::Clamp(CurrentSpreadAngle + WeaponData->Spread.SpreadPerShot, GetEffectiveBaseSpread(), WeaponData->Spread.MaxSpreadAngle);
	CurrentRecoilMultiplier = FMath::Clamp(CurrentRecoilMultiplier + WeaponData->Recoil.RecoilKickPerShot, 1.0f, WeaponData->Recoil.MaxRecoilMultiplier);
	ApplyRecoil();

	if (bDrawDebugShot)
	{
		const FVector AimPoint = GetCameraAimPoint();

		DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShotDirection * 3000.0f, FColor::Green, false, 1.0f, 0, 1.5f);
		DrawDebugSphere(GetWorld(), AimPoint, 6.0f, 8, FColor::Red, false, 1.0f);
	}
}

void AEZWeaponBase::ConsumeRound()
{
	if (bRoundChambered)
	{
		bRoundChambered = false;
	}
	if (CurrentMagazineAmmo > 0)
	{
		CurrentMagazineAmmo--;
		bRoundChambered = true;
	}
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
	const int32 Capacity = GetMagazineCapacity();

	int32 AmmoNeededForMagazine = Capacity - CurrentMagazineAmmo;
	int32 AmmoTakenFromReserve = 0;
	
	if (!bRoundChambered && ReserveAmmo > 0)
	{
		bRoundChambered = true;
		ReserveAmmo--;
		AmmoTakenFromReserve++;
	}

	const int32 AmmoForMagazine = FMath::Min(AmmoNeededForMagazine, ReserveAmmo);
	CurrentMagazineAmmo = FMath::Clamp(CurrentMagazineAmmo + AmmoForMagazine, 0, Capacity);
	ReserveAmmo -= AmmoForMagazine;
	AmmoTakenFromReserve += AmmoForMagazine;

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
	return CurrentMagazineData ? CurrentMagazineData->Capacity : 30;
}

float AEZWeaponBase::GetReloadDuration() const
{
	if (!CurrentMagazineData)
	{
		return 2.2f;
	}

	const bool bEmptyGun = !bRoundChambered && CurrentMagazineAmmo <= 0;
	return bEmptyGun ? CurrentMagazineData->EmpryReloadTime : CurrentMagazineData->ReloadTime;
}