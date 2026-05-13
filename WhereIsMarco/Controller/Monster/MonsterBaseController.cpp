// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterBaseController.h"

#include "AdvancedSightFunctionLibrary.h"
#include "BrainComponent.h"
#include "NavigationSystem.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AISense_Hearing.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/Controller/Player/MainPlayerController.h"

AMonsterBaseController::AMonsterBaseController(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("CrowdFollowingComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent")));
}

void AMonsterBaseController::BeginPlay()
{
	Super::BeginPlay();
	if (AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>())
	{
		gameState->OnRoundStateChanged.AddUniqueDynamic(this, &AMonsterBaseController::OnRoundStateChanged);
	}
}

void AMonsterBaseController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// ReturnLastTargetPosition();
	// ClearCheckedAreaMarkers();

	for (AActor* marker : _checkedAreaMarkers)
	{
		marker->Destroy();
	}
	_checkedAreaMarkers.Empty();
	_targetLastPositionMarker->Destroy();
	
	if (AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>())
	{
		gameState->OnRoundStateChanged.RemoveDynamic(this, &AMonsterBaseController::OnRoundStateChanged);
	}
}

void AMonsterBaseController::Tick(float DeltaSeconds)
{
	if (_monsterBehaviour != nullptr && IsValid(BrainComponent) && BrainComponent->IsRunning())
	{
		Super::Tick(DeltaSeconds);
		ProcessTargets();
	}
}

void AMonsterBaseController::PlaceCheckedAreaMarker(FVector location)
{
	FTransform checkedAreaMarkerTransform;
	checkedAreaMarkerTransform.SetLocation(location);
	
	AActor* checkedMarker = _checkedAreaMarkers[_currentCheckedMarker];
	checkedMarker->SetActorTransform(checkedAreaMarkerTransform);
	_currentCheckedMarker = (_currentCheckedMarker + 1) % _maxCheckedMarkers;
}

void AMonsterBaseController::ClearCheckedAreaMarkers()
{
	for (AActor* checkedAreaMarker : _checkedAreaMarkers)
	{
		FVector location = FVector::ZeroVector - FVector(0,0,10000);
		checkedAreaMarker->SetActorLocation(location);
		_currentCheckedMarker = 0;
	}
}

void AMonsterBaseController::AttachLastTargetPosition(APawn* target_pawn)
{
	if (_isLastPositionMarkerAttached)
	{
		DetachLastTargetPosition();
	}
	
	FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, false);
	_targetLastPositionMarker->AttachToActor(target_pawn, attachRules);
	_isLastPositionMarkerAttached = true;
}

void AMonsterBaseController::DetachLastTargetPosition()
{
	FDetachmentTransformRules detachRules(EDetachmentRule::KeepWorld, false);
	_targetLastPositionMarker->DetachFromActor(detachRules);
	_isLastPositionMarkerAttached = false;
}

void AMonsterBaseController::ReturnLastTargetPosition()
{
	// if (_targetLastPositionMarker != nullptr)
	// {
	// 	if (_isLastPositionMarkerAttached)
	// 	{
	// 		DetachLastTargetPosition();
	// 	}
	// 	
	// 	UrdInstSubsystem* rdInstSubsystem = GEngine ? GEngine->GetEngineSubsystem<UrdInstSubsystem>() : nullptr;
	// 	if (IsValid(rdInstSubsystem))
	// 	{
	// 		rdInstSubsystem->rdReturnActorToPool(_targetLastPositionMarker);
	// 		_targetLastPositionMarker = nullptr;
	// 	}
	// }
}

bool AMonsterBaseController::DistanceFromActorInRange(AActor* target_actor, float within_distance)
{
	return FVector::Distance(GetPawn()->GetActorLocation(), target_actor->GetActorLocation()) < within_distance;
}

bool AMonsterBaseController::CanPathToActor(AActor* target_actor)
{
	UNavigationSystemV1* navSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if (IsValid(navSystem))
	{
		FNavLocation resultingLocation;
		if (IsValid(target_actor))
		{
			return navSystem->ProjectPointToNavigation(target_actor->GetActorLocation(), resultingLocation);
		}
	}
	return false;
}

void AMonsterBaseController::TeleportToLocation(FVector new_location)
{
	GetPawn()->SetActorLocation(new_location);
}

void AMonsterBaseController::OnRoundStateChanged(ERoundState new_roundstate)
{
	switch (new_roundstate)
	{
	case ERoundState::INPROGRESS:
		if (_monsterBehaviour != nullptr)
		{
			RunBehaviorTree(_monsterBehaviour);
		}
		break;
	default:
		if (_monsterBehaviour != nullptr && IsValid(BrainComponent) && BrainComponent->IsRunning())
		{
			BrainComponent->StopLogic("Stopping brain, round is not in progress");
		}
		break;
	}
}

void AMonsterBaseController::ProcessTargets()
{
	TArray<TObjectPtr<APlayerState>> playerArray = GetWorld()->GetGameState<AWhereIsMarcoGameState>()->PlayerArray;
	if (_playerSpotStates.Num() < playerArray.Num())
	{
		for (APlayerState* playerState : playerArray)
		{
			if (_playerSpotStates.Find(playerState) == nullptr)
			{
				_playerSpotStates.Add(TTuple<TObjectPtr<APlayerState>, bool>(playerState, false));
			}
		}
	}
	
	APawn* myPawn = GetPawn();
	double closestProximityDistance = -1;
	double closestVisibleDistance = -1;

	APawn* closestProximityPawn = nullptr;
	APawn* closestVisiblePawn = nullptr;
	
	for (APlayerState* playerState : playerArray)
	{
		AMainPlayerController* playerController = Cast<AMainPlayerController>(playerState->GetPlayerController());
		APawn* playerPawn = playerController->GetPawn();

		if (!IsValid(playerController) || !IsValid(playerPawn))
		{
			continue;
		}
		
		double proximityDistance = (playerPawn->GetActorLocation() - myPawn->GetActorLocation()).Length();
		if (closestProximityDistance == -1 || proximityDistance < closestProximityDistance)
		{
			closestProximityDistance = proximityDistance;
			closestProximityPawn = playerPawn;
		}

		bool previousSpotState = _playerSpotStates[playerState];
		bool newSpotState = playerController->CanSeeActor(myPawn);
		
		_playerSpotStates[playerState] = newSpotState;

		if (previousSpotState != newSpotState)
		{
			OnTargetSpottedEvent(playerPawn, newSpotState);
		}
		
		if (newSpotState)
		{
			double visibleDistance = (playerPawn->GetActorLocation() - myPawn->GetActorLocation()).Length();
			if (closestVisibleDistance == -1 || visibleDistance < closestVisibleDistance)
			{
				closestVisibleDistance = visibleDistance;
				closestVisiblePawn = playerPawn;
			}
		}
	}

	if (_closestProximityPlayer != closestProximityPawn)
	{
		_closestProximityPlayer = closestProximityPawn;
		OnCloseProximityPlayerChanged(_closestProximityPlayer, _closestProximityPlayer != nullptr);
	}
	
	if (_closestVisiblePlayer != closestVisiblePawn)
	{
		_closestVisiblePlayer = closestVisiblePawn;
		OnCloseVisiblePlayerChanged(_closestVisiblePlayer, _closestVisiblePlayer != nullptr);
	}
}

void AMonsterBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	_monsterCharacter = Cast<AMonsterBaseCharacter>(InPawn);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterBaseController::OnPerception);

	AWhereIsMarcoGameMode* gameMode = Cast<AWhereIsMarcoGameMode>(GetWorld()->GetAuthGameMode());
	if (AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>())
	{
		for (int i = 0; i < _maxCheckedMarkers; ++i)
		{
			FVector location = FVector::ZeroVector - FVector(0,0,10000);
			FRotator rotator = FRotator::ZeroRotator;
			
			AActor* marker = GetWorld()->SpawnActor(_checkedAreaMarkerClass, &location, &rotator);
			_checkedAreaMarkers.Add(marker);
		}

		FVector location = FVector::ZeroVector - FVector(0,0,10000);
		FRotator rotator = FRotator::ZeroRotator;
			
		AActor* seenMarker = GetWorld()->SpawnActor(_targetLastPositionMarkerClass, &location, &rotator);
		_targetLastPositionMarker = seenMarker;
	}
}

void AMonsterBaseController::OnPerception(AActor* actor_detected, FAIStimulus stimulus)
{
	if (GetTeamAttitudeTowards(*actor_detected) != ETeamAttitude::Hostile || actor_detected == GetPawn())
	{
		return;
	}
	
	FAISenseID hearingSense = UAISense_Hearing::GetSenseID<UAISense_Hearing>();
	
	if (stimulus.IsActive())
	{
		if (stimulus.Type == hearingSense)
		{
			UE_LOG(LogTemp, Display, TEXT("Heard %s."), *actor_detected->GetName());
		}
	}
	else
	{
		if (stimulus.Type == hearingSense)
		{
			UE_LOG(LogTemp, Display, TEXT("Unheard %s."), *actor_detected->GetName());
		}
	}
}

ETeamAttitude::Type AMonsterBaseController::GetTeamAttitudeTowards(const AActor& other_actor) const
{
	return UAdvancedSightFunctionLibrary::GetAttitudeBetweenActors(GetPawn(), &other_actor);
}
