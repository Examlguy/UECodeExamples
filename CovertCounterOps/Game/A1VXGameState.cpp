#include "A1VXGameState.h"

#include "MainPlayerState.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "CovertCounterOps/Controllers/SoldierAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void A1VXGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(A1VXGameState, _CurrentObjective);
	DOREPLIFETIME(A1VXGameState, _OverallAlertState);
	DOREPLIFETIME(A1VXGameState, _RoundState);
	DOREPLIFETIME(A1VXGameState, _DefenderPlayers);
	DOREPLIFETIME(A1VXGameState, _AttackerPlayers);
	DOREPLIFETIME(A1VXGameState, _SpectatingPlayers);
	DOREPLIFETIME(A1VXGameState, _PlayersReady);
	DOREPLIFETIME(A1VXGameState, _TimeLimit);
	DOREPLIFETIME(A1VXGameState, _TimeStarted);
	DOREPLIFETIME(A1VXGameState, _PerceivedAttackersAtLarge);
}

void A1VXGameState::RequestTeamSelectForPlayer(APlayerState* Player, ETeam DesiredTeam)
{
	if (!IsValid(Player) || _RoundState != ERoundState::TEAM_SELECT)
	{
		return;
	}
	
	_DefenderPlayers.RemoveSwap(Player);
	_AttackerPlayers.RemoveSwap(Player);
	_SpectatingPlayers.RemoveSwap(Player);
	
	switch(DesiredTeam)
	{
	case ETeam::DEFENDER:
		_DefenderPlayers.Add(Player);
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			OnRep_DefenderTeamUpdated();
		}
		break;
	case ETeam::ATTACKER:
		_AttackerPlayers.Add(Player);
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			OnRep_AttackerTeamUpdated();
		}
		break;
	default:
		_SpectatingPlayers.Add(Player);
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			OnRep_SpectatorTeamUpdated();
		}
		break;
	}
}

void A1VXGameState::GenerateObjectives()
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}
	
	_ObjectivesInUse.Empty();
	
	TArray<AActor*> foundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectiveSet::StaticClass(),foundActors);

	int maxNumObjectives = UKismetMathLibrary::Clamp(foundActors.Num(), 0, 6);
	for (int numObjectives = 0; numObjectives < maxNumObjectives; numObjectives++)
	{
		int randomIndex = UKismetMathLibrary::RandomIntegerInRange(0, foundActors.Num() - 1);
		AObjectiveSet* objectiveSet = Cast<AObjectiveSet>(foundActors[randomIndex]);
		if (IsValid(objectiveSet))
		{
			objectiveSet->GenerateObjective();
			_ObjectivesInUse.Add(objectiveSet);
		}
		foundActors.RemoveAt(randomIndex);
	}
}

void A1VXGameState::SetOverallAlertState(EAlertState NewAlertState)
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}

	_OverallAlertState = NewAlertState;
	OnRep_OverallAlertStateChanged();
}

void A1VXGameState::ProgressToNextObjective()
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}
	
	AObjective* lastObjective = _CurrentObjective;
	if (_ObjectivesInUse.Num() > 0)
	{
		_CurrentObjective = _ObjectivesInUse.Pop()->GetSelectedObjective();
		OnRep_CurrentObjectiveChanged(lastObjective);
	}
	else
	{
		_CurrentObjective = nullptr;
		OnRep_CurrentObjectiveChanged(lastObjective);
		UE_LOG(LogTemp,Warning,TEXT("Attackers finished objectives!"));
	}
}

bool A1VXGameState::TryChangeRoundState(ERoundState NewRoundState)
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return false;
	}
	
	bool canProgress = false;
	switch (NewRoundState)
	{
	case ERoundState::SETUP_PHASE:
		canProgress = _RoundState == ERoundState::TEAM_SELECT;
		break;
	case ERoundState::BEGINNING:
		canProgress = _RoundState == ERoundState::SETUP_PHASE;
		break;
	case ERoundState::INPROGRESS:
		canProgress = _RoundState == ERoundState::BEGINNING;
		break;
	case ERoundState::FINISHED:
		canProgress = _RoundState == ERoundState::INPROGRESS;
		break;
	default:
		canProgress = true;
		break;
	}
	
	if (canProgress)
	{
		_RoundState = NewRoundState;
		OnRep_RoundStateChanged();
	}

	return canProgress;
}

bool A1VXGameState::HasAttackersLost()
{
	for (auto attacker : _AttackerPlayers)
	{
		if (AMainPlayerState* player = Cast<AMainPlayerState>(attacker))
		{
			if (player->GetPlayerPlayState() == EPlayerPlayState::ALIVE)
			{
				return false;
			}
		}
	}

	return true;
}

bool A1VXGameState::HasDefendersLost()
{
	for (auto objective : _ObjectivesInUse)
	{
		if (objective->GetSelectedObjective()->GetObjectiveCompleted() == false)
		{
			return false;
		}
	}

	return true;
}

void A1VXGameState::OnPlayerWantsToReady(AMainPlayerState* MainPlayerState)
{
	_PlayersReady.AddUnique(MainPlayerState);

	if (AreValidPlayersReady() && _TimerHandle.IsValid())
	{
		OnValidPlayersReadyDelegate.Broadcast();
	}
}

bool A1VXGameState::AreValidPlayersReady()
{
	return _AttackerPlayers.Num() + _DefenderPlayers.Num() == _PlayersReady.Num();
}

void A1VXGameState::ClearReadyPlayers()
{
	_PlayersReady.Empty();
}

void A1VXGameState::NetMulticast_NotifyAttackerEvent_Implementation(APawn* Target, ESpotState SpottedTargetState)
{
	NotifyAttackerEvent(Target, SpottedTargetState);
}

void A1VXGameState::StartTimer(float TimeLimit, TFunction<void()> OnCompleteFunction)
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}
	
	if (_TimerHandle.IsValid())
	{
		_TimerHandle.Invalidate();
	}
	
	_TimeLimit = TimeLimit;
	_TimeStarted = GetWorld()->GetTimeSeconds();
	GetWorldTimerManager().SetTimer(_TimerHandle, MoveTemp(OnCompleteFunction), TimeLimit, false);
}

void A1VXGameState::StopTimer()
{
	if (_TimerHandle.IsValid())
	{
		_TimerHandle.Invalidate();
	}
}

void A1VXGameState::OnObjectiveCompleted(AObjective* CompletedObjective)
{
	if (CompletedObjective == _CurrentObjective)
	{
		ProgressToNextObjective();
	}
}

void A1VXGameState::OnRep_PlayersReadyUpdated()
{
}

void A1VXGameState::OnRep_DefenderTeamUpdated()
{
	OnTeamsUpdatedDelegate.Broadcast();
}

void A1VXGameState::OnRep_AttackerTeamUpdated()
{
	OnTeamsUpdatedDelegate.Broadcast();
}

void A1VXGameState::OnRep_SpectatorTeamUpdated()
{
	OnTeamsUpdatedDelegate.Broadcast();
}

void A1VXGameState::OnRep_OverallAlertStateChanged()
{
	OnOverallAlertStateChangedDelegate.Broadcast(_OverallAlertState);
}

void A1VXGameState::OnRep_CurrentObjectiveChanged(AObjective* LastObjective)
{
	if (IsValid(LastObjective))
	{
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			LastObjective->OnObjectiveCompletedDelegate.RemoveDynamic(this, &A1VXGameState::OnObjectiveCompleted);
		}
		LastObjective->HideObjectiveUI();
	}
	
	if (IsValid(_CurrentObjective))
	{
		if (UKismetSystemLibrary::IsServer(GetWorld()))
		{
			_CurrentObjective->OnObjectiveCompletedDelegate.AddUniqueDynamic(this, &A1VXGameState::OnObjectiveCompleted);
		}
		_CurrentObjective->ShowObjectiveUI();
	}
}

void A1VXGameState::OnRep_RoundStateChanged()
{
	OnRoundStateChangedDelegate.Broadcast(_RoundState);
}

void A1VXGameState::OnRep_TimeLimitSet()
{
	OnTimeLimitSetDelegate.Broadcast();
}

void A1VXGameState::OnRep_TimeStartedSet()
{
	_TimeStarted += Cast<AMainPlayerController>(GetWorld()->GetFirstPlayerController())->_SingleTripTime;
}

void A1VXGameState::OnRep_TargetsAtLargeChanged()
{
	SpottedTargetsAtLargeChangedDelegate.Broadcast();
}
