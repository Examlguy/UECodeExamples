#include "PlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GMCGoldSrcMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WhereIsMarco/WhereIsMarcoGameMode.h"
#include "WhereIsMarco/Controller/Player/MainPlayerController.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	_capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	_capsuleComponent->SetupAttachment(RootComponent);

	_skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	_skeletalMeshComponent->SetupAttachment(_capsuleComponent);

	_cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	_cameraComponent->SetupAttachment(_skeletalMeshComponent);
	_cameraComponent->SetRelativeLocation(GetPawnViewLocation());

	_abilitySystemComponent = CreateDefaultSubobject<UGMC_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	_playerMovementComponent = Cast<UPlayerGoldSourceMovementComponent>(GetMovementComponent());
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerCharacter::SetMovementEnabled(bool enabled)
{
	Super::SetMovementEnabled(enabled);
	UGMC_GoldSrcMovementCmp* movementComponent = Cast<UGMC_GoldSrcMovementCmp>(GetMovementComponent());
	if (IsValid(movementComponent) && enabled == false && (movementComponent->IsCrouching() || movementComponent->IsCrouchingDown()))
	{
		CrouchActionEvent();
	}
}

void APlayerCharacter::LookAction(const FInputActionValue& input_value)
{
	Super::LookAction(input_value);
	if (_movementEnabled && _playerMovementComponent)
	{
		_playerMovementComponent->ReceiveLookInput(input_value);
	}
}

void APlayerCharacter::MoveAction(const FInputActionValue& input_value)
{
	Super::MoveAction(input_value);
	if (_movementEnabled && _playerMovementComponent)
	{
		_playerMovementComponent->ReceiveMovementInput(input_value);
	}
}
