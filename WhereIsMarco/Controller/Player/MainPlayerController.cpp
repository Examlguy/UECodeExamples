// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WhereIsMarco/WhereIsMarcoGameMode.h"
#include "WhereIsMarco/Character/Player/SpectatorCharacter.h"

AMainPlayerController::AMainPlayerController()
{
}

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (_shouldProcessLookAtTarget)
	{
		ProcessLookAtTarget(DeltaTime);
	}

	if (_shouldZoomIn || (PlayerCameraManager->GetFOVAngle() != _defaultFOVAngle && _shouldZoomIn == false))
	{
		ProcessZoom(DeltaTime);
	}
}

void AMainPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	if (AWhereIsMarcoGameMode* gameMode = GetWorld()->GetAuthGameMode<AWhereIsMarcoGameMode>())
	{
		gameMode->ReceiveReadyPlayerState(GetPlayerState<AWhereIsMarcoPlayerState>());
	}
}

void AMainPlayerController::LookAtSceneComponent(USceneComponent* look_location, float duration)
{
	if (_shouldProcessLookAtTarget == false && IsValid(look_location))
	{
		_lookAtTarget = look_location;
		_currentPossessedCharacter->SetMovementEnabled(false);
		_shouldProcessLookAtTarget = true;

		if (duration > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(_transitionTimer, [&]{ StopLookAtTarget(); }, duration, false);
		}
	}
}

void AMainPlayerController::StopLookAtTarget()
{
	if (_shouldProcessLookAtTarget == true)
	{
		_currentPossessedCharacter->SetMovementEnabled(true);
		_shouldProcessLookAtTarget = false;
	}
}

void AMainPlayerController::ZoomIn()
{
	if (_shouldZoomIn == false)
	{
		_shouldZoomIn = true;
	}
}

void AMainPlayerController::ZoomOut()
{
	if (_shouldZoomIn == true)
	{
		_shouldZoomIn = false;
	}
}

void AMainPlayerController::SetPlayerFOV(float new_angle)
{
	_defaultFOVAngle = new_angle;
	if (AWhereIsMarcoPlayerState* playerState = GetPlayerState<AWhereIsMarcoPlayerState>())
	{
		playerState->SetFOVAngle(new_angle);
	}
}

bool AMainPlayerController::IsLookingAtActor(AActor* looking_actor)
{
	if (!IsValid(looking_actor))
	{
		return false;
	}
	
	FVector directionFromSelf = UKismetMathLibrary::GetDirectionUnitVector(GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector());
	directionFromSelf.Normalize();
	FVector directionToActor = UKismetMathLibrary::GetDirectionUnitVector(GetPawn()->GetActorLocation(), looking_actor->GetActorLocation());
	directionToActor.Normalize();
	float angleToActor = FMath::RadiansToDegrees(acosf(FVector::DotProduct(directionFromSelf, directionToActor)));

	return angleToActor <= _defaultFOVAngle;
}

bool AMainPlayerController::CanSeeActor(AActor* target_actor)
{
	if (!IsValid(_currentPossessedCharacter))
	{
		return false;
	}
	
	FVector visibilityOffset(0,0,0);
	if (AMonsterBaseCharacter* monster = Cast<AMonsterBaseCharacter>(target_actor))
	{
		visibilityOffset = monster->GetEyeHeightOffset();
	}

	FHitResult hitResult;
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(GetPawn());
	
	GetWorld()->LineTraceSingleByChannel(hitResult,_currentPossessedCharacter->GetPawnViewLocation(), target_actor->GetActorLocation() + visibilityOffset, ECC_Visibility, collisionParams);
	if (hitResult.GetActor() == target_actor || hitResult.bBlockingHit == false)
	{
		return true;
	}
	
	return false;
}

void AMainPlayerController::ProcessLookAtTarget(float delta_time)
{
	FRotator currentRotation = GetControlRotation();
	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(_currentPossessedCharacter->GetActorLocation(), _lookAtTarget->GetComponentLocation() - _currentPossessedCharacter->GetEyeHeightOffset());
	
	SetControlRotation(UKismetMathLibrary::RInterpTo(currentRotation, targetRotation, delta_time, 10.0f));
}

void AMainPlayerController::ProcessZoom(float delta_time)
{
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	SetPlayerFOV(PlayerCameraManager->GetFOVAngle() - 10.0f);
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Triggered, this, &AMainPlayerController::PrimaryActionTriggered);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Triggered, this, &AMainPlayerController::SecondaryActionTriggered);
		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Triggered, this, &AMainPlayerController::EscapeActionTriggered);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMainPlayerController::CrouchActionTriggered);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMainPlayerController::SprintActionTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::MoveInput);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::LookInput);
	}
}

void AMainPlayerController::OnPossess(APawn* in_pawn)
{
	Super::OnPossess(in_pawn);
	APossessableCharacter* possessableCharacter = Cast<APossessableCharacter>(in_pawn);
	if (IsValid(possessableCharacter))
	{
		_currentPossessedCharacter = possessableCharacter;
	}
	
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(in_pawn);
	if (IsValid(playerCharacter))
	{
		GetPlayerState<AWhereIsMarcoPlayerState>()->Multicast_SetCharacterState(EPlayerCharacterState::ALIVE);
	}
	
	ASpectatorCharacter* spectatorCharacter = Cast<ASpectatorCharacter>(in_pawn);
	if (IsValid(spectatorCharacter))
	{
		GetPlayerState<AWhereIsMarcoPlayerState>()->Multicast_SetCharacterState(EPlayerCharacterState::SPECTATING);
	}
}

void AMainPlayerController::AcknowledgePossession(APawn* in_pawn)
{
	Super::AcknowledgePossession(in_pawn);
	APossessableCharacter* possessableCharacter = Cast<APossessableCharacter>(in_pawn);
	if (IsValid(possessableCharacter))
	{
		_currentPossessedCharacter = possessableCharacter;
	}

	ASpectatorCharacter* spectatorCharacter = Cast<ASpectatorCharacter>(in_pawn);
	if (IsValid(spectatorCharacter))
	{
		spectatorCharacter->FindNextSpectatableActor();
	}

	if (_shouldProcessLookAtTarget)
	{
		StopLookAtTarget();
	}

	OnClientPossessionChangedDelegate.Broadcast(in_pawn);
}

void AMainPlayerController::MoveInput(const FInputActionValue& input_value)
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->MoveAction(input_value);
	}
}

void AMainPlayerController::LookInput(const FInputActionValue& input_value)
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->LookAction(input_value);
	}
}

void AMainPlayerController::PrimaryActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->PrimaryAction();
	}
}

void AMainPlayerController::SecondaryActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->SecondaryAction();
	}
}

void AMainPlayerController::CrouchActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->CrouchAction();
	}
}

void AMainPlayerController::SprintActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->SprintAction();
	}
}

void AMainPlayerController::InteractActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->InteractionAction();
	}
}

void AMainPlayerController::EscapeActionTriggered()
{
	if (IsValid(_currentPossessedCharacter))
	{
		_currentPossessedCharacter->EscapeAction();
	}
}
