// Fill out your copyright notice in the Description page of Project Settings.

#include "AIUtilityComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UAIUtilityComponent::UAIUtilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAIUtilityComponent::SelectActor(bool IsSelected)
{
	OnActorSelectedDelegate.Broadcast(GetOwner(), IsSelected);
}

void UAIUtilityComponent::SetFocusRequest(FVector FocusLocation, bool HasFocusLocation)
{
	_FocusLocation = FocusLocation;
	_HasFocusLocation = HasFocusLocation;
}

void UAIUtilityComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAIUtilityComponent, _AlertState);
	DOREPLIFETIME(UAIUtilityComponent, _SpotState);
}


// Called when the game starts
void UAIUtilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>())
	{
		gameState->OnOverallAlertStateChangedDelegate.AddUniqueDynamic(this, &UAIUtilityComponent::OnOverallAlertStateChanged);
	}
}


// Called every frame
void UAIUtilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (_PatrolPoints.Num() > 0)
	{
		FVector firstPatrolPoint = _PatrolPoints[0];
		FVector lastPatrolPoint = _PatrolPoints[0];
		
		for (auto location : _PatrolPoints)
		{
			DrawDebugLine(GetWorld(), lastPatrolPoint, location, FColor::White, false, -1, 0, 5);
			DrawDebugSphere(GetWorld(), location, 50, 4, FColor::White);
			lastPatrolPoint = location;
		}
		
		DrawDebugLine(GetWorld(), lastPatrolPoint, firstPatrolPoint, FColor::White, false, -1, 0, 5);
	}
}

void UAIUtilityComponent::FindAndSetNearestPatrolPointFirst()
{
	if (!IsValid(GetOwner()))
	{
		return;
	}
	
	if (_PatrolPoints.Num() == 0 || _PatrolPoints.Num() == 1)
	{
		_PatrolStartIndex = 0;
		return;
	}

	int closestIndex = 0;
	FVector closestLocation = _PatrolPoints[closestIndex];
	
	for (int index = 0; index < _PatrolPoints.Num(); index++)
	{
		FVector testLocation = _PatrolPoints[index];
		if (FVector::Dist(GetOwner()->GetActorLocation(), testLocation) < FVector::Dist(GetOwner()->GetActorLocation(), closestLocation))
		{
			closestIndex = index;
			closestLocation = testLocation;
		}
	}

	_PatrolStartIndex = closestIndex;
}

void UAIUtilityComponent::AddPatrolPoint(FVector& PatrolPoint, bool Additive)
{
	if (!Additive)
	{
		ClearPatrolPoints();
	}
	
	_PatrolPoints.Add(PatrolPoint);
	OnPatrolPointsUpdatedDelegate.Broadcast();
}

FVector UAIUtilityComponent::GetNextPatrolPoint()
{
	if (!IsValid(GetOwner()))
	{
		return FVector::ZeroVector;
	}
	
	if (_PatrolPoints.Num() == 0)
	{
		return GetOwner()->GetActorLocation();
	}
	
	FVector nextPoint = _PatrolPoints[_PatrolStartIndex];
	_PatrolStartIndex = (_PatrolStartIndex + 1) % _PatrolPoints.Num();
	
	return nextPoint;
}

void UAIUtilityComponent::ClearPatrolPoints()
{
	_PatrolStartIndex = 0;
	_PatrolPoints.Empty();
}

void UAIUtilityComponent::OnOverallAlertStateChanged(EAlertState NewOverallAlertState)
{
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		_AlertState = NewOverallAlertState;
		OnRep_AlertState();
	}
}

void UAIUtilityComponent::OnTargetPerceptionState(APawn* SeenBy, APawn* Target, ESpotState SpotState)
{
	if (!IsValid(Target))
	{
		return;
	}
	
	_SpotState = SpotState;
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		OnRep_SpotState();
	}

	switch (_SpotState)
	{
	case ESpotState::NONE:
		break;
	case ESpotState::FORGOT:
		if (Target == _CurrentTarget)
		{
			_CurrentTarget = nullptr;
		}
		break;
	case ESpotState::LOST:
		if (Target == _CurrentTarget)
		{
			_CurrentTarget = nullptr;
		}
		break;
	case ESpotState::SPOTTED:
		if (_AlertState < EAlertState::MEDIUM)
		{
			_AlertState = EAlertState::MEDIUM;
			if (UKismetSystemLibrary::IsServer(GetWorld()))
			{
				OnRep_AlertState();
			}
		}
		else if (_AlertState == EAlertState::MEDIUM)
		{
			_AlertState = EAlertState::HIGH;
			if (UKismetSystemLibrary::IsServer(GetWorld()))
			{
				OnRep_AlertState();
			}
		}
		_CurrentTarget = Target;
		break;
	case ESpotState::PERCEIVED:
		if (_AlertState < EAlertState::ALARMED)
		{
			_AlertState = EAlertState::ALARMED;
			if (UKismetSystemLibrary::IsServer(GetWorld()))
			{
				OnRep_AlertState();
			}
		}
		_CurrentTarget = Target;
		break;
	}
}

void UAIUtilityComponent::OnRep_AlertState()
{
	AlertStateChangedEvent();
}

void UAIUtilityComponent::OnRep_SpotState()
{
	SpotStateChangedEvent();
}