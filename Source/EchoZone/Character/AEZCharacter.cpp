#include "AEZCharacter.h"
#include "Component/UEZCharacterMovementComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

AEZCharacter::AEZCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UEZCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 0.0f;
    SpringArmComponent->bUsePawnControlRotation = true;

    ViewRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ViewRoot"));
    ViewRootComponent->SetupAttachment(SpringArmComponent);

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(ViewRootComponent);
    CameraComponent->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    TargetViewZ = StandingViewZ;
}

void AEZCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }
    }

    if (ViewRootComponent)
    {
        FVector RelativeLocation = ViewRootComponent->GetRelativeLocation();
        RelativeLocation.Z = StandingViewZ;
        ViewRootComponent->SetRelativeLocation(RelativeLocation);
    }

    UpdateLeanState();
    UpdateMovementBlockers();

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->RefreshMovementSettings();
    }
}

void AEZCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateFreeLook(DeltaTime);
    UpdateView(DeltaTime);
}

void AEZCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEZCharacter::Move);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AEZCharacter::Move);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AEZCharacter::Move);
        }

        if (LookAction)
        {
            EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEZCharacter::Look);
        }

        if (SprintAction)
        {
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AEZCharacter::StartSprint);
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AEZCharacter::StopSprint);
            EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopSprint);
        }

        if (CrouchAction)
        {
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AEZCharacter::StartCrouch);
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AEZCharacter::StopCrouch);
            EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopCrouch);
        }

        if (LeanLeftAction)
        {
            EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Started, this, &AEZCharacter::StartLeanLeft);
            EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Completed, this, &AEZCharacter::StopLeanLeft);
            EnhancedInputComponent->BindAction(LeanLeftAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopLeanLeft);
        }

        if (LeanRightAction)
        {
            EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Started, this, &AEZCharacter::StartLeanRight);
            EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Completed, this, &AEZCharacter::StopLeanRight);
            EnhancedInputComponent->BindAction(LeanRightAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopLeanRight);
        }

        if (FreeLookAction)
        {
            EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &AEZCharacter::StartFreeLook);
            EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &AEZCharacter::StopFreeLook);
            EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Canceled, this, &AEZCharacter::StopFreeLook);
        }

        if (WalkSpeedUpAction)
        {
            EnhancedInputComponent->BindAction(WalkSpeedUpAction, ETriggerEvent::Started, this, &AEZCharacter::IncreaseWalkSpeedStep);
        }

        if (WalkSpeedDownAction)
        {
            EnhancedInputComponent->BindAction(WalkSpeedDownAction, ETriggerEvent::Started, this, &AEZCharacter::DecreaseWalkSpeedStep);
        }
    }
}

UEZCharacterMovementComponent* AEZCharacter::GetUEZMovementComponent() const
{
    return Cast<UEZCharacterMovementComponent>(GetCharacterMovement());
}

void AEZCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetMoveInput(MoveInput);
    }

    if (!Controller)
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MoveInput.Y);
    AddMovementInput(RightDirection, MoveInput.X);
}

void AEZCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();

    if (bIsFreeLooking)
    {
        FreeLookYaw = FMath::Clamp(FreeLookYaw + LookAxis.X, -FreeLookYawLimit, FreeLookYawLimit);
        FreeLookPitch = FMath::Clamp(FreeLookPitch + LookAxis.Y, -FreeLookPitchDownLimit, FreeLookPitchUpLimit);
    }
    else
    {
        AddControllerYawInput(LookAxis.X);
        AddControllerPitchInput(LookAxis.Y);
    }
}

void AEZCharacter::StartSprint()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(true);
    }
}

void AEZCharacter::StopSprint()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(false);
    }
}

void AEZCharacter::StartCrouch()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(false);
    }

    if (!bIsCrouched)
    {
        Crouch();
    }
}

void AEZCharacter::StopCrouch()
{
    if (bIsCrouched)
    {
        UnCrouch();
    }
}

void AEZCharacter::StartLeanLeft()
{
    bIsLeaningLeft = true;
    bIsLeaningRight = false;

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(false);
    }

    UpdateLeanState();
    UpdateMovementBlockers();
}

void AEZCharacter::StopLeanLeft()
{
    bIsLeaningLeft = false;

    UpdateLeanState();
    UpdateMovementBlockers();
}

void AEZCharacter::StartLeanRight()
{
    bIsLeaningRight = true;
    bIsLeaningLeft = false;

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(false);
    }

    UpdateLeanState();
    UpdateMovementBlockers();
}

void AEZCharacter::StopLeanRight()
{
    bIsLeaningRight = false;

    UpdateLeanState();
    UpdateMovementBlockers();
}

void AEZCharacter::StartFreeLook()
{
    bIsFreeLooking = true;

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintIntent(false);
    }

    UpdateMovementBlockers();
}

void AEZCharacter::StopFreeLook()
{
    bIsFreeLooking = false;
    UpdateMovementBlockers();
}

void AEZCharacter::IncreaseWalkSpeedStep()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->IncreaseWalkSpeedStep();
    }
}

void AEZCharacter::DecreaseWalkSpeedStep()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->DecreaseWalkSpeedStep();
    }
}

void AEZCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

    TargetViewZ = CrouchedViewZ;

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->RefreshMovementSettings();
    }
}

void AEZCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

    TargetViewZ = StandingViewZ;

    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->RefreshMovementSettings();
    }
}

void AEZCharacter::UpdateLeanState()
{
    if (bIsLeaningLeft && !bIsLeaningRight)
    {
        TargetLeanRoll = -LeanAngle;
        TargetLeanOffsetY = -LeanOffset;
    }
    else if (bIsLeaningRight && !bIsLeaningLeft)
    {
        TargetLeanRoll = LeanAngle;
        TargetLeanOffsetY = LeanOffset;
    }
    else
    {
        TargetLeanRoll = 0.0f;
        TargetLeanOffsetY = 0.0f;
    }
}

void AEZCharacter::UpdateMovementBlockers()
{
    if (UEZCharacterMovementComponent* EZMoveComp = GetUEZMovementComponent())
    {
        EZMoveComp->SetSprintBlockedByFreeLook(bIsFreeLooking);
        EZMoveComp->SetSprintBlockedByLean(bIsLeaningLeft || bIsLeaningRight);
    }
}

void AEZCharacter::UpdateView(float DeltaTime)
{
    if (!ViewRootComponent)
    {
        return;
    }

    const FVector CurrentLocation = ViewRootComponent->GetRelativeLocation();
    const FRotator CurrentRotation = ViewRootComponent->GetRelativeRotation();

    const FVector TargetLocation(0.0f, TargetLeanOffsetY, TargetViewZ);
    const FRotator TargetRotation(0.0f, FreeLookYaw, TargetLeanRoll);

    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, ViewHeightInterpSpeed);
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, LeanInterpSpeed);

    if (bIsFreeLooking)
    {
        NewRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, FreeLookPitch, DeltaTime, LeanInterpSpeed);
    }
    else
    {
        NewRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
    }

    ViewRootComponent->SetRelativeLocation(NewLocation);
    ViewRootComponent->SetRelativeRotation(NewRotation);
}

void AEZCharacter::UpdateFreeLook(float DeltaTime)
{
    if (bIsFreeLooking)
    {
        return;
    }

    FreeLookYaw = FMath::FInterpTo(FreeLookYaw, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
    FreeLookPitch = FMath::FInterpTo(FreeLookPitch, 0.0f, DeltaTime, FreeLookReturnInterpSpeed);
}