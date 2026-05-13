// Copyright Epic Games, Inc. All Rights Reserved.

#include "A1VXGameMode.h"

#include "A1VXGameState.h"
#include "CovertCounterOpsWorldSettings.h"
#include "CovertCounterOps/Characters/SpectatorCharacter.h"
#include "CovertCounterOps/Characters/Interfaces/CommonUtility.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "CovertCounterOps/Controllers/SoldierAIController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

A1VXGameMode::A1VXGameMode()
{
	PlayerStateClass = AMainPlayerState::StaticClass();
	DefaultPawnClass = ASpectatorCharacter::StaticClass();
	SpectatorClass = ASpectatorCharacter::StaticClass();
}

void A1VXGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void A1VXGameMode::BeginPlay()
{
	Super::BeginPlay();

	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (IsValid(gameState))
	{
		gameState->OnValidPlayersReadyDelegate.AddUniqueDynamic(this, &A1VXGameMode::AllValidPlayersReady);
		gameState->OnRoundStateChangedDelegate.AddUniqueDynamic(this, &A1VXGameMode::OnRoundStateChanged);
	}
	
	RestartRound();
}

void A1VXGameMode::RestartRound()
{
	StartMatch();
	CleanupSpawnedRoundActors();

	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || !gameState->TryChangeRoundState(ERoundState::TEAM_SELECT))
	{
		return;
	}
	
	ACovertCounterOpsWorldSettings* worldSettings = Cast<ACovertCounterOpsWorldSettings>(GetWorldSettings());
	FVector spawnLocation = FVector::ZeroVector;
	FRotator spawnRotation = FRotator::ZeroRotator;
	
	if (IsValid(worldSettings))
	{
		worldSettings->ShowAllFloors();
		FTransform spawnTransform = worldSettings->GetDefenderSpawnTransform();
		spawnLocation = spawnTransform.GetLocation();
	}
	
	for (APlayerState* playerState : gameState->PlayerArray)
	{
		gameState->RequestTeamSelectForPlayer(playerState, ETeam::UNASSIGNED);
		APawn* pawn = RespawnPlayerAsClass(playerState, SpectatorClass, spawnLocation, spawnRotation);
		if (pawn->Implements<UCommonUtility>())
		{
			ICommonUtility::Execute_SetTeam(pawn, static_cast<int32>(ETeam::UNASSIGNED));
		}
	}
	
	gameState->GenerateObjectives();
}

void A1VXGameMode::StartSetupPhase()
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || (gameState->GetDefenderPlayers().Num() == 0 && gameState->GetAttackerPlayers().Num() == 0))
	{
		return;
	}
	
	if (!gameState->TryChangeRoundState(ERoundState::SETUP_PHASE))
	{
		return;
	}

	for (APlayerState* playerState : gameState->PlayerArray)
	{
		ACovertCounterOpsWorldSettings* worldSettings = Cast<ACovertCounterOpsWorldSettings>(GetWorldSettings());
		FVector spawnLocation = FVector::ZeroVector;
		FRotator spawnRotation = FRotator::ZeroRotator;

		if (IsValid(worldSettings))
		{
			FTransform spawnTransform = worldSettings->GetDefenderSpawnTransform();
			spawnLocation = spawnTransform.GetLocation();
			spawnRotation = FRotator(spawnTransform.GetRotation());
		}

		bool isDefender = gameState->IsDefenderPlayer(playerState);
		bool isAttacker = gameState->IsAttackerPlayer(playerState);

		if (isDefender || isAttacker)
		{
			if (APawn* setupPawn = RespawnPlayerAsClass(playerState, _SetupCharacter, spawnLocation, spawnRotation))
			{
				if (AMainPlayerState* mainState = Cast<AMainPlayerState>(playerState))
				{
					mainState->SetPlayerPlayState(EPlayerPlayState::BUSY);
				}

				if (setupPawn->Implements<UCommonUtility>())
				{
					ICommonUtility::Execute_SetTeam(setupPawn, static_cast<int32>(isDefender ? ETeam::DEFENDER : ETeam::ATTACKER));
				}
			}
		}
	}
}

void A1VXGameMode::EndSetupPhase()
{
	if (A1VXGameState* gameState = GetGameState<A1VXGameState>())
	{
		if (!gameState->TryChangeRoundState(ERoundState::BEGINNING))
		{
			return;
		}
	}
	
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), _SoldierCharacter, allActors);
	_AllDefenderSoldiers.Empty();
	
	for (auto actor : allActors)
	{
		_AllDefenderSoldiers.Add(Cast<APawn>(actor));
		if (APawn* pawn = Cast<APawn>(actor))
		{
			if (ASoldierAIController* soldierController = Cast<ASoldierAIController>(pawn->GetController()))
			{
				soldierController->OnTargetPerceptionStateDelegate.AddUniqueDynamic(this, &A1VXGameMode::OnPlayerSpottedEvent);
			}
		}
	}
}

void A1VXGameMode::StartRound()
{
	if (A1VXGameState* gameState = GetGameState<A1VXGameState>())
	{
		if (!gameState->TryChangeRoundState(ERoundState::INPROGRESS))
		{
			return;
		}
		
		HandleStartingSpawns();
		gameState->ProgressToNextObjective();
	}
}

void A1VXGameMode::EndRound()
{
	if (A1VXGameState* gameState = GetGameState<A1VXGameState>())
	{
		if (!gameState->TryChangeRoundState(ERoundState::FINISHED))
		{
			return;
		}
	}
}

void A1VXGameMode::HandleStartingSpawns()
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}
	
	{
		for (APlayerState* playerState : GetGameState<A1VXGameState>()->GetAttackerPlayers())
		{
			if (AMainPlayerState* mainPlayerState = Cast<AMainPlayerState>(playerState))
			{
				FRotator lookAtLocation = FRotator::ZeroRotator;
				if (IsValid(gameState->GetCurrentObjective()))
				{
					lookAtLocation = UKismetMathLibrary::FindLookAtRotation(mainPlayerState->GetDesiredSpawnPosition(), gameState->GetCurrentObjective()->GetActorLocation());
					lookAtLocation.Pitch = 0;
					lookAtLocation.Roll = 0;
				}
			
				if (APawn* attackerPawn = RespawnPlayerAsClass(playerState, _AttackerCharacter, mainPlayerState->GetDesiredSpawnPosition(), lookAtLocation))
				{
					if (attackerPawn->Implements<UCommonUtility>())
					{
						ICommonUtility::Execute_SetTeam(attackerPawn, static_cast<int32>(ETeam::ATTACKER));
					}
					
					AddSpawnedRoundActor(attackerPawn);
					mainPlayerState->SetPlayerPlayState(EPlayerPlayState::ALIVE);
					_AllAttackerCharacters.Add(attackerPawn);
				}
			}
		}
	}

	{	// Must spawn after attacker players, because when players possess the RTS controller, they will hide the attackers
		FVector spawnLocation = FVector::ZeroVector;
		FRotator spawnRotation = FRotator::ZeroRotator;

		for (APlayerState* playerState : GetGameState<A1VXGameState>()->GetDefenderPlayers())
		{
			if (APawn* setUpPawn = playerState->GetPawn())
			{
				spawnLocation = setUpPawn->GetActorLocation();
				spawnRotation = setUpPawn->GetActorRotation();
			}
			
			if (AMainPlayerState* mainPlayerState = Cast<AMainPlayerState>(playerState))
			{
				if (APawn* defenderPawn = RespawnPlayerAsClass(playerState, _DefenderCharacter, spawnLocation, spawnRotation))
				{
					if (defenderPawn->Implements<UCommonUtility>())
					{
						ICommonUtility::Execute_SetTeam(defenderPawn, static_cast<int32>(ETeam::DEFENDER));
					}
					
					mainPlayerState->SetPlayerPlayState(EPlayerPlayState::ALIVE);
					_AllDefenderCharacters.Add(defenderPawn);
				}
			}
		}
	}
}

void A1VXGameMode::BindToPlayerState(AMainPlayerState* MainPlayerState)
{
	if (IsValid(MainPlayerState))
	{
		MainPlayerState->OnPlayerPlayStateChangedDelegate.AddUniqueDynamic(this, &A1VXGameMode::OnPlayerPlayStateChanged);
		A1VXGameState* gameState = GetGameState<A1VXGameState>();
		if (IsValid(gameState))
		{
			MainPlayerState->OnPlayerWantsToReadyDelegate.AddUniqueDynamic(gameState, &A1VXGameState::OnPlayerWantsToReady);
		}
	}
}

void A1VXGameMode::OnPlayerPlayStateChanged(AMainPlayerState* MainPlayerState, EPlayerPlayState NewState)
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}
	
	switch (NewState)
	{
	case EPlayerPlayState::DEAD:
		if (gameState->GetRoundState() == ERoundState::INPROGRESS)
		{
			RespawnPlayerAsClass(MainPlayerState, SpectatorClass, MainPlayerState->GetPawn()->GetActorLocation(), MainPlayerState->GetPawn()->GetActorRotation(), false);
		}
		break;
	case EPlayerPlayState::SPECTATING:
		if (gameState->GetRoundState() == ERoundState::INPROGRESS)
		{
			if (gameState->HasAttackersLost())
			{
				gameState->TryChangeRoundState(ERoundState::FINISHED);
			}
		}
		break;
	default:
		break;
	}
}

void A1VXGameMode::AllValidPlayersReady()
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}

	ERoundState currentRoundState = gameState->GetRoundState();
	switch (currentRoundState)
	{
	case ERoundState::SETUP_PHASE:
		EndSetupPhase();
		break;
	case ERoundState::BEGINNING:
		StartRound();
		break;
	case ERoundState::INPROGRESS:
		break;
	case ERoundState::FINISHED:
		break;
	case ERoundState::POST_GAME:
		RestartRound();
		break;
	default:
		break;
	}

	gameState->StopTimer();
	gameState->ClearReadyPlayers();
}

void A1VXGameMode::OnRoundStateChanged(ERoundState NewRoundState)
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}
	
	bool startTimer = false;
	float timerLimit = 0;
	TFunction<void()> onTimerFinish;
	
	switch (NewRoundState)
	{
	case ERoundState::SETUP_PHASE:
		startTimer = true;
		timerLimit = 180.f;
		onTimerFinish = [this]()
		{
			EndSetupPhase();
		};
		break;
	case ERoundState::BEGINNING:
		startTimer = true;
		timerLimit = 10.f;
		onTimerFinish = [this]()
		{
			StartRound();
		};
		break;
	case ERoundState::INPROGRESS:
		startTimer = true;
		timerLimit = 1800.f;
		onTimerFinish = [this]()
		{
			A1VXGameState* gameState = GetGameState<A1VXGameState>();
			if (!IsValid(gameState))
			{
				return;
			}
			gameState->TryChangeRoundState(ERoundState::FINISHED);
		};
		break;
	case ERoundState::FINISHED:
		startTimer = true;
		timerLimit = 5.f;
		onTimerFinish = [this]()
		{
			A1VXGameState* gameState = GetGameState<A1VXGameState>();
			if (!IsValid(gameState))
			{
				return;
			}
			gameState->TryChangeRoundState(ERoundState::POST_GAME);
		};
		break;
	case ERoundState::POST_GAME:
		startTimer = true;
		timerLimit = 10.f;
		onTimerFinish = [this]()
		{
			RestartRound();
		};
		break;
	default:
		break;
	}

	if (startTimer)
	{
		gameState->StartTimer(timerLimit, onTimerFinish);
	}
	else
	{
		gameState->StopTimer();
	}
}

void A1VXGameMode::KillPlayer(AMainPlayerController* PlayerController)
{
	RespawnPlayerAsClass(PlayerController->GetPlayerState<APlayerState>(), SpectatorClass, PlayerController->GetPawn()->GetActorLocation(), PlayerController->GetPawn()->GetActorRotation());
}

APawn* A1VXGameMode::RespawnPlayerAsClass(APlayerState* PlayerState, UClass* NewClass, FVector SpawnLocation, FRotator SpawnRotation, bool DestroyOldPawn)
{
	if (APawn* newPawn = Cast<APawn>(GetWorld()->SpawnActor(NewClass, &SpawnLocation)))
	{
		newPawn->SetActorRotation(SpawnRotation);
		if (AController* controller = newPawn->GetController())
		{
			controller->UnPossess();
			controller->Destroy();
		}
			
		if (AMainPlayerController* PlayerController = Cast<AMainPlayerController>(PlayerState->GetPlayerController()))
		{
			if (APawn* PreviousPawn = PlayerController->GetPawn())
			{
				PlayerController->UnPossess();
				if (DestroyOldPawn)
				{
					PreviousPawn->Destroy();
				}
			}
			
			PlayerController->Possess(newPawn);
			PlayerController->SetMainControlledPawn(newPawn);
		}

		return newPawn;
	}

	return nullptr;
}

void A1VXGameMode::OnPlayerSpottedEvent(APawn* SeenBy, APawn* Target, ESpotState SpotState)
{
	A1VXGameState* gameState = GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}

	AMainPlayerState* playerState = Cast<AMainPlayerState>(Target->GetPlayerState());
	if (IsValid(playerState))
	{
		switch (SpotState)
		{
		case ESpotState::SPOTTED:
			playerState->_SpottedByEnemies.AddUnique(SeenBy);
			break;
		case ESpotState::PERCEIVED:
			playerState->_SpottedByEnemies.RemoveSwap(SeenBy);
			playerState->_PerceivedByEnemies.AddUnique(SeenBy);
			gameState->_PerceivedAttackersAtLarge.AddUnique(Target);
			break;
		default:
			playerState->_SpottedByEnemies.RemoveSwap(SeenBy);
			playerState->_PerceivedByEnemies.RemoveSwap(SeenBy);
			break;
		}

		gameState->NetMulticast_NotifyAttackerEvent(Target, SpotState);
	}
}

void A1VXGameMode::AddSpawnedRoundActor(AActor* SpawnedActor)
{
	_SpawnedRoundActors.Add(SpawnedActor);
}

bool A1VXGameMode::IsSpawnedRoundActor(AActor* SpawnedActor)
{
	return _SpawnedRoundActors.Find(SpawnedActor) != INDEX_NONE;
}

void A1VXGameMode::RemoveSpawnedRoundActor(AActor* ActorToRemove)
{
	_SpawnedRoundActors.Remove(ActorToRemove);
}

void A1VXGameMode::CleanupSpawnedRoundActors()
{
	for (int i = 0; i < _SpawnedRoundActors.Num(); i++)
	{
		if (IsValid(_SpawnedRoundActors[i]))
		{
			if (APawn* pawn = Cast<APawn>(_SpawnedRoundActors[i]))
			{
				AController* controller = pawn->GetController();
				if (IsValid(controller))
				{
					if (ASoldierAIController* soldierController = Cast<ASoldierAIController>(controller))
					{
						soldierController->OnTargetPerceptionStateDelegate.RemoveDynamic(this, &A1VXGameMode::OnPlayerSpottedEvent);
					}
					controller->UnPossess();
					controller->Destroy();
				}
			}
			
			_SpawnedRoundActors[i]->Destroy();
		}
	}
	_SpawnedRoundActors.Empty();
}