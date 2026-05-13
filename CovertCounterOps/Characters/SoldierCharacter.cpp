// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoldierCharacter.h"

#include "RTSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "CovertCounterOps/Controllers/SoldierAIController.h"
#include "CovertCounterOps/Game/A1VXGameMode.h"
#include "CovertCounterOps/Items/Activatable.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogSoldierCharacter);

ASoldierCharacter::ASoldierCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	_SoldierAIUtility = CreateDefaultSubobject<UAIUtilityComponent>(TEXT("SoldierAIUtilityComponent"));
	
	_TeamComponent = CreateDefaultSubobject<UAdvancedSightTeamComponent>(TEXT("TeamComponent"));
	_TeamNumber = static_cast<int32>(ETeam::DEFENDER);
	_TeamComponent->TeamID = _TeamNumber;
	
	_SightTargetComponent = CreateDefaultSubobject<UAdvancedSightTargetComponent>(TEXT("SightTargetComponent"));
}

void ASoldierCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CheckForInteractable();
}

void ASoldierCharacter::SetTeam_Implementation(int32 TeamNumber)
{
	_TeamNumber = TeamNumber;
	OnRep_TeamNumberChanged();
}

float ASoldierCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	DamageApplied = FMath::Min(_CurrentHealth, DamageApplied);
	_CurrentHealth -= DamageApplied;

	NetMulticast_HasTakenDamage(DamageApplied);
	
	if (_CurrentHealth <= 0 && _SoldierCharacterState != EActorState::DEAD)
	{
		_TeamComponent->DestroyComponent();
		_SightTargetComponent->DestroyComponent();
		
		if (IsPawnControlled())
		{
			if (IsPlayerControlled())
			{
				if (AMainPlayerState* playerState = GetController()->GetPlayerState<AMainPlayerState>())
				{
					playerState->SetPlayerPlayState(EPlayerPlayState::DEAD);
				}
			}
			else
			{
				AController* controller = GetController();
				controller->UnPossess();
				controller->Destroy();
			}
		}

		_SoldierCharacterState = EActorState::DEAD;
		
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			OnRep_SoldierCharacterState();
		}
	}

	return DamageApplied;
}

void ASoldierCharacter::NetMulticast_HasTakenDamage_Implementation(float DamageAmount)
{
	OnSimulateTakenDamageEventDelegate.Broadcast(DamageAmount);
}

void ASoldierCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASoldierCharacter, _TeamNumber);
	DOREPLIFETIME(ASoldierCharacter, _SoldierCharacterState);
	DOREPLIFETIME(ASoldierCharacter, _CurrentHeldItem);
	DOREPLIFETIME(ASoldierCharacter, _CurrentHealth);
}

void ASoldierCharacter::CheckForInteractable()
{
	if (!IsValid(_CameraComponent))
	{
		return;
	}
	
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, _CameraComponent->GetComponentLocation(), _CameraComponent->GetComponentLocation() + _CameraComponent->GetForwardVector() * _InteractDistance, ECollisionChannel::ECC_Visibility);

	if (Hit.GetActor() != _CurrentInteractable.Get())
	{
		if (AActivatable* activateable = Cast<AActivatable>(Hit.GetActor()))
		{
			_CurrentInteractable = activateable;
			OnInteractableLookedAtChangedDelegate.Broadcast(activateable);
		}
		else
		{
			if (_CurrentInteractable != nullptr)
			{
				_CurrentInteractable->StopHoldTrigger(this);
				Server_RequestStopHoldTrigger(_CurrentInteractable.Get());
				
				_CurrentInteractable = nullptr;
				OnInteractableLookedAtChangedDelegate.Broadcast(nullptr);
			}
		}
	}
	
	if (!Hit.bBlockingHit)
	{
		if (_CurrentInteractable != nullptr)
		{
			_CurrentInteractable = nullptr;
			OnInteractableLookedAtChangedDelegate.Broadcast(nullptr);
		}
	}
}

void ASoldierCharacter::InteractInteractable()
{
	if (CanInteractInteractable() && IsValid(_CurrentInteractable.Get()))
	{
		_CurrentInteractable->PawnTriggeredActivate(this, !_CurrentInteractable->GetActivated());
		Server_InteractInteractable(_CurrentInteractable.Get());
	}
}

void ASoldierCharacter::Server_RequestStopHoldTrigger_Implementation(AActivatable* Activatable)
{
	Activatable->StopHoldTrigger(this);
}

bool ASoldierCharacter::CanInteractInteractable_Implementation()
{
	return true;
}

void ASoldierCharacter::Server_InteractInteractable_Implementation(AActivatable* Activatable)
{
	if (CanInteractInteractable() && IsValid(Activatable) && FMath::Abs(FVector::Distance(Activatable->GetActorLocation(), GetActorLocation())) <= (_InteractDistance + 100.f))
	{
		Activatable->PawnTriggeredActivate(this, !Activatable->GetActivated());
	}
}

void ASoldierCharacter::Server_DropItem_Implementation(AItem* DropItem)
{
	if (DropItem == _CurrentHeldItem.Get())
	{
		_CurrentHeldItem = nullptr;
	}
	
	OnRep_CurrentHeldItem(DropItem);
}

void ASoldierCharacter::Server_PickUpItem_Implementation(AItem* PickedUpItem)
{
	AItem* lastItem = _CurrentHeldItem.Get();
	_CurrentHeldItem = PickedUpItem;
	OnRep_CurrentHeldItem(lastItem);
}

void ASoldierCharacter::OnRep_TeamNumberChanged()
{
	_TeamComponent->SwitchTeam(_TeamNumber);
}

void ASoldierCharacter::BeginPlay()
{
	_SkeletalMeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	_CameraComponent = FindComponentByClass<UCameraComponent>();

	Super::BeginPlay();
}

void ASoldierCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (IsValid(_CurrentHeldItem.Get()))
	{
		_CurrentHeldItem.Get()->Destroy();
	}
}

void ASoldierCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		if (!IsValid(_SoldierAIUtility))
		{
			UE_LOG(LogTemp, Error, TEXT("AIUtility is not VALID!"));
			return;
		}
		
		if (ASoldierAIController* soldierController = Cast<ASoldierAIController>(GetController()))
		{
			soldierController->OnTargetPerceptionStateDelegate.AddUniqueDynamic(_SoldierAIUtility, &UAIUtilityComponent::OnTargetPerceptionState);
		}
		else
		{
			if (ASoldierAIController* previousController = Cast<ASoldierAIController>(PreviousController))
			{
				previousController->OnTargetPerceptionStateDelegate.RemoveDynamic(_SoldierAIUtility, &UAIUtilityComponent::OnTargetPerceptionState);
			}
			
			if (AMainPlayerController* playerController = Cast<AMainPlayerController>(GetController()))
			{
				_SoldierAIUtility->DestroyComponent();
			}
		}
	}
}

void ASoldierCharacter::OnRep_SoldierCharacterState()
{
	SoldierCharacterStateChangedEvent();
}

void ASoldierCharacter::OnRep_CurrentHeldItem(TSoftObjectPtr<AItem> LastItem)
{
	if (IsValid(_CurrentHeldItem.Get()))
	{
		_CurrentHeldItem.Get()->Pickup(this, _SkeletalMeshComponent, _ItemHoldingBoneName);
		OnPickedUpItemEvent();
	}
	else
	{
		if (IsValid(LastItem.Get()))
		{
			LastItem->Drop();
		}
		OnDropItemEvent();
	}
}
