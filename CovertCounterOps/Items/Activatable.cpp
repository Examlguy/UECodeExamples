#include "Activatable.h"

#include "GameFramework/GameStateBase.h"
#include "Camera/CameraComponent.h"
#include "CovertCounterOps/Characters/Interfaces/CommonUtility.h"
#include "Net/UnrealNetwork.h"

AActivatable::AActivatable()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

float AActivatable::GetPercentageHeldTrigger() const
{
	return IsValid(_triggerHoldingPawn) ? FMath::Clamp((GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - _startHoldTimeForActivate) / _holdTimeForActivate, 0, 1) : 0.f;
}

void AActivatable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AActivatable, _activatableState);
	DOREPLIFETIME(AActivatable, _triggerHoldingPawn);
}

void AActivatable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckIfPlayerStillHoldingTrigger();
}

void AActivatable::StartHoldTrigger(APawn* instigating_pawn)
{
	if (IsValid(_triggerHoldingPawn))
	{
		return;
	}
	
	_startHoldTimeForActivate = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	_triggerHoldingPawn = instigating_pawn;
	
	OnHoldActivationEvent(true);
}

void AActivatable::StopHoldTrigger(APawn* instigating_pawn)
{
	OnHoldActivationEvent(false);
	if (_triggerHoldingPawn == instigating_pawn)
	{
		_triggerHoldingPawn = nullptr;
	}
}

void AActivatable::CheckIfPlayerStillHoldingTrigger()
{
	if (IsValid(_triggerHoldingPawn))
	{
		if (!_triggerHoldingPawn->Implements<UCommonUtility>())
		{
			return;
		}
		
		UCameraComponent* cameraComponent = _triggerHoldingPawn->FindComponentByClass<UCameraComponent>();
		
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), cameraComponent->GetComponentLocation(), ECollisionChannel::ECC_Visibility);

		if (Hit.bBlockingHit && Hit.GetActor()->GetOwner() != cameraComponent->GetOwner())
		{
			UE_LOG(LogTemp, Warning, TEXT("Something got in the way"));
			_triggerHoldingPawn = nullptr;
			return;
		}
		
		if (FVector::Distance(cameraComponent->GetComponentLocation(), GetActorLocation()) > ICommonUtility::Execute_GetInteractDistance(_triggerHoldingPawn) + 100.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Walked too far from the objective"));
			_triggerHoldingPawn = nullptr;
			return;
		}
		
		// Dirty way to check to make sure we're looking at the objective, but good enough for now
		if (FVector::DotProduct(cameraComponent->GetForwardVector(), GetActorForwardVector()) >= 0.45 && FMath::IsNearlyEqual(GetPercentageHeldTrigger(),1, 0.01f))
		{
			UE_LOG(LogTemp, Warning, TEXT("Finished Hold Activate"));
			ToggleActivate(true);
			StopHoldTrigger(_triggerHoldingPawn);
		}
	}
}

void AActivatable::PawnTriggeredActivate(APawn* instigating_pawn, bool requested_active)
{
	if (_isHoldToActivate && _activatableState.activated == false)
	{
		if (_triggerHoldingPawn == nullptr)
		{
			StartHoldTrigger(instigating_pawn);
			UE_LOG(LogTemp, Warning, TEXT("Starting hold activate"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Called hold activate but pawn was null"));
		}
		return;
	}
	
	if (requested_active != _activatableState.activated && ((requested_active == true && _playerActivatable) || (requested_active == false && _playerDeactivatable)))
	{
		FActivatableState newState;
		newState.activated = requested_active;
		newState.instigatingPawn = instigating_pawn;
		newState.time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		SetActivated(newState);
	}
}

void AActivatable::ToggleActivate(bool requested_active)
{
	FActivatableState newState;
	newState.activated = requested_active;
	newState.time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	SetActivated(newState);
}

void AActivatable::OnRepNotify_ActivatedChanged()
{
	OnActivationChangedEvent();
}

void AActivatable::Multicast_ActivatableCorrection_Implementation(FActivatableState correct_activatable_state)
{
	UE_LOG(LogTemp, Warning, TEXT("Correction"))
	_activatableState = correct_activatable_state;
	OnRepNotify_ActivatedChanged();
}

void AActivatable::BeginPlay()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FActivatableState newState;
		newState.activated = _startActivated;
		SetActivated(newState);
	}
	Super::BeginPlay();
}

void AActivatable::SetActivated(FActivatableState activated_state)
{
	if (activated_state.time < _activatableState.time && !activated_state.correction)
	{
		Multicast_ActivatableCorrection(_activatableState);
		return;
	}

	_activatableState = activated_state;
	OnRepNotify_ActivatedChanged();
}
