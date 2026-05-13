// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

enum class ETeam : uint8;

UENUM(BlueprintType)
enum class EPlayerPlayState : uint8
{
	UNASSIGNED = 0,
	BUSY,
	ALIVE,
	DEAD,
	SPECTATING
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPlayStateChangedDelegate, AMainPlayerState*, PlayerState, EPlayerPlayState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWantsToReadyDelegate, AMainPlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerDesiredSpawnpointSetDelegate, AMainPlayerState*, PlayerState, FVector, SpawnLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerPerceptionStateChangedDelegate);

UCLASS()
class COVERTCOUNTEROPS_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	FVector GetDesiredSpawnPosition() const { return _DesiredSpawnPosition; }

	UFUNCTION(BlueprintCallable)
	EPlayerPlayState GetPlayerPlayState() const { return _PlayerPlayState; }
	
	UFUNCTION(BlueprintCallable)
	void SetPlayerPlayState(EPlayerPlayState NewPlayerPlayState)
	{
		_PlayerPlayState = NewPlayerPlayState;
		OnRep_PlayerPlayState();
	}

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_ConfirmReadyUp(AMainPlayerState* FromPlayerState);
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestReadyUp();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_RequestSetDesiredSpawnPosition(FVector_NetQuantize SpawnPosition);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestTeamSelect(ETeam DesiredTeam);

	UFUNCTION(BlueprintCallable)
	void SetDesiredSpawnPoint(FVector SpawnLocation);
	
	UFUNCTION()
	void OnRep_PlayerPlayState();

	UFUNCTION()
	void OnRep_PlayerDesiredSpawnpointSet();

	UFUNCTION()
	void OnRep_SpottedByEnemyChanged();
	
	UFUNCTION()
	void OnRep_PerceivedByEnemyChanged();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsSpottedByAnyEnemies();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPerceivedByAnyEnemies();
	
	UPROPERTY(ReplicatedUsing=OnRep_SpottedByEnemyChanged, BlueprintReadOnly)
	TArray<APawn*> _SpottedByEnemies;

	UPROPERTY(ReplicatedUsing=OnRep_PerceivedByEnemyChanged, BlueprintReadOnly)
	TArray<APawn*> _PerceivedByEnemies;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerPlayStateChangedDelegate OnPlayerPlayStateChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerWantsToReadyDelegate OnPlayerWantsToReadyDelegate;

	UPROPERTY(BlueprintAssignable)
	FPlayerPerceptionStateChangedDelegate PlayerPerceptionStateChangedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDesiredSpawnpointSetDelegate OnPlayerDesiredSpawnpointSetDelegate;
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_PlayerDesiredSpawnpointSet)
	FVector _DesiredSpawnPosition = FVector::ZeroVector;
	FVector _LastDesiredPosition = FVector::ZeroVector;
	float _DesiredSpawnPointSetTime = 0;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerPlayState)
	EPlayerPlayState _PlayerPlayState = EPlayerPlayState::UNASSIGNED;
};
