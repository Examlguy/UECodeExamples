// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorCharacter.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "WhereIsMarco/WhereIsMarcoPlayerState.h"
#include "WhereIsMarco/World/WhereIsMarcoWorldSettings.h"


// Sets default values
ASpectatorCharacter::ASpectatorCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpectatorCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASpectatorCharacter::FindNextSpectatableActor()
{
	for (APlayerState* playerState : GetWorld()->GetGameState()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (_currentSpectatingPlayerState != marcoPlayerState && (marcoPlayerState->GetCharacterState() == EPlayerCharacterState::ALIVE ||
				marcoPlayerState->GetCharacterState() == EPlayerCharacterState::DYING))
			{
				if (IsValid(_currentSpectatingPlayerState))
				{
					_currentSpectatingPlayerState->OnCharacterStateChanged.RemoveDynamic(this,  &ASpectatorCharacter::OnPlayerCharacterStateChanged);
				}
				
				marcoPlayerState->OnCharacterStateChanged.AddUniqueDynamic(this, &ASpectatorCharacter::OnPlayerCharacterStateChanged);
				_currentSpectatingPlayerState = marcoPlayerState;
				if (APawn* pawn = marcoPlayerState->GetPawn())
				{
					_currentSpectatingComponent = Cast<USkeletalMeshComponent>(pawn->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
				}
				return;
			}
		}
	}

	if (!IsValid(_currentSpectatingPlayerState))
	{
		_currentSpectatingComponent = Cast<USceneComponent>(Cast<AWhereIsMarcoWorldSettings>(GetWorld()->GetWorldSettings())->GetSpectatorFallback()->GetComponentByClass(USceneComponent::StaticClass()));
	}
}

void ASpectatorCharacter::OnPlayerCharacterStateChanged(AWhereIsMarcoPlayerState* player_state, EPlayerCharacterState new_state)
{
	switch (new_state)
	{
		case EPlayerCharacterState::DEAD:
			if (IsValid(_currentSpectatingPlayerState))
			{
				_currentSpectatingPlayerState->OnCharacterStateChanged.RemoveDynamic(this,  &ASpectatorCharacter::OnPlayerCharacterStateChanged);
			}
			_currentSpectatingPlayerState = nullptr;
			_currentSpectatingComponent = nullptr;
			FindNextSpectatableActor();
			break;
		default:
			break;
	}
}

void ASpectatorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(_currentSpectatingComponent))
	{
		FVector spectatePosition = _currentSpectatingComponent->GetComponentLocation();

		if (IsValid(_currentSpectatingPlayerState))
		{
			spectatePosition -= FVector(0,0, _currentSpectatingComponent->GetRelativeLocation().Z);
		}
		
		SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), spectatePosition, DeltaTime, 20.0f));
	}
}

void ASpectatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

