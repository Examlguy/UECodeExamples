#include "MonsterBaseCharacter.h"
#include "Components/CapsuleComponent.h"

AMonsterBaseCharacter::AMonsterBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	_capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	_capsuleComponent->SetupAttachment(RootComponent);
	
	_scareLookAtLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ScareLookAtLocation"));
	_scareLookAtLocation->SetupAttachment(_capsuleComponent);
}

void AMonsterBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonsterBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonsterBaseCharacter::ChangeMovementState(EMovementState movement_state)
{
	_currentMovementState = movement_state;
	MovementStateChangedEvent(movement_state);
}
