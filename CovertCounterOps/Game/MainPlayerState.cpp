#include "MainPlayerState.h"

#include "A1VXGameState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

void AMainPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>())
	{
		gameState->PlayerStateInitialised(this);
	}
}

void AMainPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainPlayerState, _DesiredSpawnPosition);
	DOREPLIFETIME(AMainPlayerState, _PlayerPlayState);
	DOREPLIFETIME(AMainPlayerState, _SpottedByEnemies);
	DOREPLIFETIME(AMainPlayerState, _PerceivedByEnemies);
}

void AMainPlayerState::NetMulticast_ConfirmReadyUp_Implementation(AMainPlayerState* FromPlayerState)
{
	OnPlayerWantsToReadyDelegate.Broadcast(FromPlayerState);
}

void AMainPlayerState::Server_RequestReadyUp_Implementation()
{
	NetMulticast_ConfirmReadyUp(this);
}

void AMainPlayerState::Server_RequestSetDesiredSpawnPosition_Implementation(FVector_NetQuantize SpawnPosition)
{
	_DesiredSpawnPosition = SpawnPosition;
	OnRep_PlayerDesiredSpawnpointSet();
}

void AMainPlayerState::Server_RequestTeamSelect_Implementation(ETeam DesiredTeam)
{
	if (A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>())
	{
		gameState->RequestTeamSelectForPlayer(this, DesiredTeam);
	}
}

void AMainPlayerState::SetDesiredSpawnPoint(FVector SpawnLocation)
{
	_DesiredSpawnPosition = SpawnLocation;
	_LastDesiredPosition = SpawnLocation;
	_DesiredSpawnPointSetTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	
	OnPlayerDesiredSpawnpointSetDelegate.Broadcast(this, _DesiredSpawnPosition);
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		OnRep_PlayerDesiredSpawnpointSet();
	}
}

void AMainPlayerState::OnRep_PlayerPlayState()
{
	OnPlayerPlayStateChangedDelegate.Broadcast(this, _PlayerPlayState);
}

void AMainPlayerState::OnRep_PlayerDesiredSpawnpointSet()
{
	if (GetWorld()->GetGameState()->GetServerWorldTimeSeconds() > _DesiredSpawnPointSetTime && _LastDesiredPosition != _DesiredSpawnPosition)
	{
		OnPlayerDesiredSpawnpointSetDelegate.Broadcast(this, _DesiredSpawnPosition);
		_LastDesiredPosition = _DesiredSpawnPosition;
		_DesiredSpawnPointSetTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	}
}

void AMainPlayerState::OnRep_SpottedByEnemyChanged()
{
	PlayerPerceptionStateChangedDelegate.Broadcast();
}

void AMainPlayerState::OnRep_PerceivedByEnemyChanged()
{
	PlayerPerceptionStateChangedDelegate.Broadcast();
}

bool AMainPlayerState::IsSpottedByAnyEnemies()
{
	return _SpottedByEnemies.Num() > 0;
}

bool AMainPlayerState::IsPerceivedByAnyEnemies()
{
	return _PerceivedByEnemies.Num() > 0;
}
