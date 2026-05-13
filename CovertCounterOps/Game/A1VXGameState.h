// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectiveSet.h"
#include "GameFramework/GameState.h"
#include "A1VXGameState.generated.h"

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	TEAM_SELECT = 0,
	SETUP_PHASE = 1,
	BEGINNING = 2,
	INPROGRESS = 3,
	FINISHED = 4,
	POST_GAME = 5
};

UENUM(Blueprintable)
enum class EAlertState : uint8
{
	LOW,
	MEDIUM,
	HIGH,
	ALARMED
};

UENUM(Blueprintable)
enum class ETeam : uint8
{
	DEFENDER = 0,
	ATTACKER = 1,
	UNASSIGNED = 255
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverallAlertStateChangedDelegate, EAlertState, NewAlertState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundStateChangedDelegate, ERoundState, NewRoundState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTeamsUpdatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FValidPlayersReadyDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimeLimitSetDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpottedTargetsAtLargeChangedDelegate);

class APlayerState;
class AMainPlayerState;

UCLASS()
class COVERTCOUNTEROPS_API A1VXGameState : public AGameState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	AObjective* GetCurrentObjective() const { return _CurrentObjective; }
	
	UFUNCTION()
	ERoundState GetRoundState() const { return _RoundState; }

	UFUNCTION(BlueprintCallable)
	TArray<APlayerState*> GetDefenderPlayers() const { return _DefenderPlayers; }
	
	UFUNCTION(BlueprintCallable)
	TArray<APlayerState*> GetAttackerPlayers() const { return _AttackerPlayers; }

	UFUNCTION(BlueprintCallable)
	bool IsDefenderPlayer(APlayerState* PlayerState) const { return _DefenderPlayers.Find(PlayerState) != INDEX_NONE; }

	UFUNCTION(BlueprintCallable)
	bool IsAttackerPlayer(APlayerState* PlayerState) const { return _AttackerPlayers.Find(PlayerState) != INDEX_NONE; }

	UFUNCTION(BlueprintCallable)
	bool IsPlayerReady(APlayerState* PlayerState) const { return _SpectatingPlayers.Find(PlayerState) != INDEX_NONE; }

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerStateInitialised(APlayerState* PlayerState);
	
	UFUNCTION(BlueprintCallable)
	void RequestTeamSelectForPlayer(APlayerState* Player, ETeam DesiredTeam);
	
	UFUNCTION()
	void GenerateObjectives();

	UFUNCTION(BlueprintCallable)
	void SetOverallAlertState(EAlertState NewAlertState);
	
	UFUNCTION(BlueprintCallable)
	EAlertState GetOverallAlertState() const { return _OverallAlertState;}
	
	UFUNCTION(BlueprintCallable)
	void ProgressToNextObjective();

	UFUNCTION(BlueprintCallable)
	bool TryChangeRoundState(ERoundState NewRoundState);

	UFUNCTION(BlueprintCallable)
	bool HasAttackersLost();
	
	UFUNCTION(BlueprintCallable)
	bool HasDefendersLost();

	UFUNCTION()
	void OnPlayerWantsToReady(AMainPlayerState* MainPlayerState);
	
	UFUNCTION(BlueprintCallable)
	bool AreValidPlayersReady();
	
	UFUNCTION(BlueprintCallable)
	void ClearReadyPlayers();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_NotifyAttackerEvent(APawn* Target, ESpotState SpottedTargetState);

	UFUNCTION(BlueprintImplementableEvent)
	void NotifyAttackerEvent(APawn* Target, ESpotState SpottedTargetState);
	
	void StartTimer(float TimeLimit, TFunction<void()> OnCompleteFunction);
	void StopTimer();
	
	UFUNCTION(BlueprintCallable)
	void OnObjectiveCompleted(AObjective* CompletedObjective);
	
	UFUNCTION()
	void OnRep_DefenderTeamUpdated();

	UFUNCTION()
	void OnRep_AttackerTeamUpdated();

	UFUNCTION()
	void OnRep_SpectatorTeamUpdated();

	UFUNCTION()
	void OnRep_PlayersReadyUpdated();
	
	UFUNCTION()
	void OnRep_CurrentObjectiveChanged(AObjective* LastObjective);
	
	UFUNCTION()
	void OnRep_OverallAlertStateChanged();

	UFUNCTION()
	void OnRep_RoundStateChanged();

	UFUNCTION()
	void OnRep_TimeLimitSet();
	
	UFUNCTION()
	void OnRep_TimeStartedSet();
	
	UFUNCTION()
	void OnRep_TargetsAtLargeChanged();
	
	UPROPERTY(BlueprintAssignable)
	FOnOverallAlertStateChangedDelegate OnOverallAlertStateChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundStateChangedDelegate OnRoundStateChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FValidPlayersReadyDelegate OnValidPlayersReadyDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FTeamsUpdatedDelegate OnTeamsUpdatedDelegate;

	UPROPERTY(BlueprintAssignable)
	FTimeLimitSetDelegate OnTimeLimitSetDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpottedTargetsAtLargeChangedDelegate SpottedTargetsAtLargeChangedDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_TargetsAtLargeChanged, BlueprintReadOnly)
	TArray<APawn*> _PerceivedAttackersAtLarge;
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_DefenderTeamUpdated, BlueprintReadWrite)
	TArray<APlayerState*> _DefenderPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_AttackerTeamUpdated, BlueprintReadWrite)
	TArray<APlayerState*> _AttackerPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_SpectatorTeamUpdated, BlueprintReadWrite)
	TArray<APlayerState*> _SpectatingPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_PlayersReadyUpdated, BlueprintReadWrite)
	TArray<APlayerState*> _PlayersReady;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AObjectiveSet*> _ObjectivesInUse;
	
	UPROPERTY(ReplicatedUsing=OnRep_OverallAlertStateChanged, BlueprintReadOnly)
	EAlertState _OverallAlertState = EAlertState::LOW;
	
	TObjectPtr<AObjectiveSet> _CurrentObjectiveSet;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentObjectiveChanged, BlueprintReadOnly)
	TObjectPtr<AObjective> _CurrentObjective;

	UPROPERTY(ReplicatedUsing=OnRep_RoundStateChanged, BlueprintReadOnly)
	ERoundState _RoundState = ERoundState::TEAM_SELECT;

	UPROPERTY(ReplicatedUsing=OnRep_TimeLimitSet, BlueprintReadOnly)
	float _TimeLimit;
	UPROPERTY(ReplicatedUsing=OnRep_TimeStartedSet, BlueprintReadOnly)
	float _TimeStarted;
	UPROPERTY()
	FTimerHandle _TimerHandle;
};
