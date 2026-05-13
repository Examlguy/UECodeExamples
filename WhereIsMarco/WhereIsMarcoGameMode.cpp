#include "WhereIsMarcoGameMode.h"

#include "GameConfigurationInstanceSubsystem.h"
#include "SteamGameInstanceSubsystem.h"
#include "WhereIsMarcoGameState.h"
#include "WhereIsMarcoPlayerState.h"
#include "Character/Player/SpectatorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpectatorPawn.h"
#include "UObject/ConstructorHelpers.h"
#include "World/WhereIsMarcoWorldSettings.h"
#include "World/AreaSpawnNode.h"
#include "World/Interactable/ObjectiveActivatable.h"
#include "NavigationSystem.h"

AWhereIsMarcoGameMode::AWhereIsMarcoGameMode()
{
	GameStateClass = AWhereIsMarcoGameState::StaticClass();
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerStateClass = AWhereIsMarcoPlayerState::StaticClass();
	SpectatorClass = ASpectatorCharacter::StaticClass();
}

void AWhereIsMarcoGameMode::StartPlay()
{
	UGameInstance* gameInstance = GetGameInstance();
	UGameConfigurationInstanceSubsystem* gameConfig = gameInstance->GetSubsystem<UGameConfigurationInstanceSubsystem>();

	gameConfig->WaitForGameShadersBuilt();
	
	UE_LOG(LogTemp, Warning, TEXT("### Gamemode Started ###"));
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectiveActivatable::StaticClass(), _objectiveActivatables);
	_marcoCharacter = Cast<AMarcoCharacter>(GetWorld()->SpawnActor(_marcoCharacterClass));

	switch (gameConfig->GetDifficulty())
	{
		case EDifficultyConfig::EASY:
			_currentDifficulty = _easyDifficulty;
			break;
		case EDifficultyConfig::NORMAL:
			_currentDifficulty = _normalDifficulty;
			break;
		case EDifficultyConfig::HARD:
			_currentDifficulty = _hardDifficulty;
			break;
		case EDifficultyConfig::EXTREME:
			_currentDifficulty = _extremeDifficulty;
			break;
	}

	GetGameState<AWhereIsMarcoGameState>()->OnPlayerWon.AddUniqueDynamic(this, &AWhereIsMarcoGameMode::OnPlayerWon);
	
	Super::StartPlay();
}

bool AWhereIsMarcoGameMode::TryFinishStage()
{
	if (_roundState != ERoundState::INPROGRESS)
	{
		return false;
	}

	SetRoundState(ERoundState::FINISHED);
	return true;
}

void AWhereIsMarcoGameMode::ForceRestartStage(bool force_respawn, bool stop_spawn_movement)
{
	SetGameStage(_gameStage);
	SetRoundState(ERoundState::BEGINNING);
	RespawnPlayers(force_respawn);
	
	AWhereIsMarcoGameState* gameState = GetGameState<AWhereIsMarcoGameState>();
	if (IsValid(gameState) && IsValid(_marcoCharacter))
	{
		if (stop_spawn_movement)
		{
			gameState->SetPlayersAllowedToMove(false);
			gameState->Multicast_SetPlayersAllowedToMove(false);
			_playersMovementDisabled = true;
		}
		
		int numberOfScreenshots = _currentDifficulty->GetNumberOfScreenshots();
		gameState->Multicast_RestartMarcoLocationTextures(numberOfScreenshots);
		_marcoCharacter->GenerateMarcoLocations(numberOfScreenshots);
		
		RespawnMonsters();
	}
}

bool AWhereIsMarcoGameMode::TryStartStage()
{
	if (_roundState != ERoundState::BEGINNING)
	{
		return false;
	}

	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (marcoPlayerState->GetCharacterState() != EPlayerCharacterState::ALIVE || !marcoPlayerState->GetIsReadyToStart())
			{
				return false;
			}
		}
	}

	UGameInstance* gameInstance = GetGameInstance();
	USteamGameInstanceSubsystem* steamSubsytem = gameInstance->GetSubsystem<USteamGameInstanceSubsystem>();
	if (IsValid(steamSubsytem) && steamSubsytem->IsCurrentlyInSession())
	{
		steamSubsytem->StartSession();
	}

	//GetGameState<AWhereIsMarcoGameState>()->Multicast_ShowMarcoLocationScreenshot(GetGameState<AWhereIsMarcoGameState>()->GetCurrentMarcoLocationKey());
	
	SetRoundState(ERoundState::INPROGRESS);
	return true;
}

bool AWhereIsMarcoGameMode::TryNextStage()
{
	if (_roundState != ERoundState::FINISHED)
	{
		return false;
	}
	
	bool hasPlayerWonRound = false;
	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (marcoPlayerState->GetPlayerWonThisRound())
			{
				hasPlayerWonRound = true;
				break;
			}
		}
	}
	
	if (hasPlayerWonRound)
	{
		_gameStage++;
	}
	
	ForceRestartStage(true, true);
	return true;
}

bool AWhereIsMarcoGameMode::TryKillPlayer(AMonsterBaseCharacter* killer_character, APlayerCharacter* player_character, float animation_duration)
{
	if (AWhereIsMarcoPlayerState* playerState = Cast<AWhereIsMarcoPlayerState>(player_character->GetPlayerState()))
	{
		if (playerState->GetCharacterState() != EPlayerCharacterState::ALIVE)
		{
			return false;
		}
		
		playerState->Multicast_SetCharacterState(EPlayerCharacterState::DYING);

		GetGameState<AWhereIsMarcoGameState>()->Multicast_KillPlayer(killer_character, player_character, animation_duration);
		
		if (animation_duration > 0.0f)
		{
			float duration = animation_duration + playerState->GetPingInMilliseconds() * 0.001;
			duration = duration < 0.0f ? 0.0f : duration;
			
			FTimerHandle timerHandle;
			GetWorld()->GetTimerManager().SetTimer(timerHandle, [=]
			{
				if (playerState->GetCharacterState() != EPlayerCharacterState::DYING)
				{
					return;
				}
				playerState->Multicast_SetCharacterState(EPlayerCharacterState::DEAD);
			}, duration, false);
		}
		else
		{
			if (playerState->GetCharacterState() != EPlayerCharacterState::DYING)
			{
				return false;
			}
			playerState->Multicast_SetCharacterState(EPlayerCharacterState::DEAD);
		}

		return true;
	}

	return false;
}

void AWhereIsMarcoGameMode::RespawnMonsters()
{
	if (_spawnedMonsters.Num() > 0)
	{
		for (AMonsterBaseCharacter* monster : _spawnedMonsters)
		{
			if (IsValid(monster))
			{
				monster->Destroy();
			}
		}
		_spawnedMonsters.Empty();
	}
	
	_spawnLocations.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAreaSpawnNode::StaticClass(), _spawnLocations);

	for (TSubclassOf<AMonsterBaseCharacter> monsterClass : _currentDifficulty->GetMonstersToSpawnAtClueNumber(_gameStage))
	{
		if (_spawnLocations.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ran out of spawn locations, refilling array."));
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAreaSpawnNode::StaticClass(), _spawnLocations);
		}
		
		int locationIndex = FMath::RandRange(0, _spawnLocations.Num() - 1);
		AAreaSpawnNode* spawnLocation = Cast<AAreaSpawnNode>(_spawnLocations[locationIndex]);
		if (IsValid(spawnLocation))
		{
			_spawnLocations.RemoveAt(locationIndex);

			FVector spawnPosition = spawnLocation->GetRandomLocationInNavigatableArea();
			FRotator spawnRotation = FRotator::ZeroRotator;
			AMonsterBaseCharacter* spawnedMonster = Cast<AMonsterBaseCharacter>(GetWorld()->SpawnActor(monsterClass, &spawnPosition, &spawnRotation));
			if (IsValid(spawnedMonster))
			{
				spawnedMonster->SetActorLocation(spawnedMonster->GetPawnViewLocation());
				_spawnedMonsters.Add(spawnedMonster);
			}
		}
	}
}

bool AWhereIsMarcoGameMode::ArePlayersStillAlive() const
{
	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (marcoPlayerState->GetCharacterState() == EPlayerCharacterState::ALIVE)
			{
				return true;
			}
		}
	}

	return false;
}

bool AWhereIsMarcoGameMode::ArePlayersStillBusy() const
{
	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (marcoPlayerState->GetCharacterState() == EPlayerCharacterState::DYING || marcoPlayerState->GetCharacterState() == EPlayerCharacterState::LIMBO)
			{
				return true;
			}
		}
	}

	return false;
}

bool AWhereIsMarcoGameMode::AreObjectivesCompleted() const
{
	for (AActor* actor : _objectiveActivatables)
	{
		AObjectiveActivatable* objective = Cast<AObjectiveActivatable>(actor);
		if (IsValid(objective) && objective->IsObjectiveAvailable() && !objective->GetActivated())
		{
			return false;
		}
	}

	return true;
}

void AWhereIsMarcoGameMode::ReceiveReadyPlayerState(AWhereIsMarcoPlayerState* player_state)
{
	player_state->OnCharacterStateChanged.AddUniqueDynamic(this, &AWhereIsMarcoGameMode::OnPlayerStateChanged);
	player_state->OnCharacterReadiedChanged.AddUniqueDynamic(this, &AWhereIsMarcoGameMode::OnPlayerReadyChanged);
}

void AWhereIsMarcoGameMode::OnPlayerReadyChanged(bool new_state)
{
	bool allPlayersReady = true;
	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			if (!marcoPlayerState->GetIsReadyToStart())
			{
				allPlayersReady = false;
				break;
			}
		}
	}

	if (allPlayersReady)
	{
		AWhereIsMarcoGameState* gameState = GetGameState<AWhereIsMarcoGameState>();
		if (IsValid(gameState))
		{
			gameState->Multicast_AllPlayersReady();

			if (_playersMovementDisabled)
			{
				gameState->SetPlayersAllowedToMove(true);
				gameState->Multicast_SetPlayersAllowedToMove(true);
			}
			
			TryStartStage();
		}
	}
}

void AWhereIsMarcoGameMode::OnPlayerStateChanged(AWhereIsMarcoPlayerState* player_state, EPlayerCharacterState new_state)
{
	switch (new_state)
	{
		case EPlayerCharacterState::DEAD:
			if (GetRoundState() == ERoundState::INPROGRESS)
			{
				RespawnPlayerAsClass(player_state, SpectatorClass, player_state->GetPawn()->GetActorLocation(), player_state->GetPawn()->GetActorRotation());
			}
			break;
		case EPlayerCharacterState::SPECTATING:
			if (GetRoundState() == ERoundState::INPROGRESS)
			{
				if (!ArePlayersStillAlive() && !ArePlayersStillBusy())
				{
					TryFinishStage();
				}
			}
			break;
		default:
			break;
	}
}

void AWhereIsMarcoGameMode::OnPlayerWon(APawn* winning_player)
{
	UGameInstance* gameInstance = GetGameInstance();
	UGameConfigurationInstanceSubsystem* gameConfig = gameInstance->GetSubsystem<UGameConfigurationInstanceSubsystem>();

	if (gameConfig->GetPlayMode() == EPlayMode::RACE)
	{
		for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
		{
			if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
			{
				if (!marcoPlayerState->GetPlayerWonThisRound())
				{
					return;
				}
			}
		}
	}

	TryFinishStage();
}

void AWhereIsMarcoGameMode::RespawnPlayers(bool force_respawn)
{
	int spawnIndex = 0;
	AWhereIsMarcoWorldSettings* worldSettings = Cast<AWhereIsMarcoWorldSettings>(GetWorldSettings());
	TArray<AActor*> spawnPoints = worldSettings->GetRespawnLocations();

	if (!IsValid(worldSettings))
	{
		return;
	}
	
	for (APlayerState* playerState : GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		if (AWhereIsMarcoPlayerState* marcoPlayerState = Cast<AWhereIsMarcoPlayerState>(playerState))
		{
			marcoPlayerState->UnreadyPlayer();
			marcoPlayerState->SetPlayerWonThisRound(false);
			
			if (!force_respawn && marcoPlayerState->GetCharacterState() == EPlayerCharacterState::ALIVE)
			{
				continue;
			}
			
			FVector spawnLocation = spawnPoints[spawnIndex]->GetActorLocation();
			FRotator spawnRotation = spawnPoints[spawnIndex]->GetActorRotation();
			spawnIndex = (spawnIndex + 1) % spawnPoints.Num();
			
			RespawnPlayerAsClass(marcoPlayerState, DefaultPawnClass, spawnLocation, spawnRotation);
		}
	}
}

void AWhereIsMarcoGameMode::RespawnPlayerAsClass(AWhereIsMarcoPlayerState* player, UClass* new_class, FVector spawn_location, FRotator spawn_rotation)
{
	if (APawn* newPawn = Cast<APawn>(GetWorld()->SpawnActor(new_class, &spawn_location)))
	{
		newPawn->SetActorRotation(spawn_rotation);
		
		if (AController* playerController = player->GetPlayerController())
		{
			newPawn->SetOwner(playerController);

			APawn* previousPawn = player->GetPawn();
			playerController->UnPossess();
			playerController->Possess(newPawn);
		
			if (IsValid(previousPawn))
			{
				previousPawn->Destroy();
			}
		}
	}
}

void AWhereIsMarcoGameMode::SetGameStage(uint8 new_game_stage)
{
	_gameStage = new_game_stage;
	GetGameState<AWhereIsMarcoGameState>()->Multicast_GameStageUpdated(new_game_stage);
}

void AWhereIsMarcoGameMode::SetRoundState(ERoundState new_round_state)
{
	_roundState = new_round_state;
	GetGameState<AWhereIsMarcoGameState>()->Multicast_RoundStateUpdated(new_round_state);
}