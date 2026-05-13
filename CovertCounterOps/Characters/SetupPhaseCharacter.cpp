#include "SetupPhaseCharacter.h"

#include "CovertCounterOps/Game/A1VXGameMode.h"
#include "CovertCounterOps/Game/MainPlayerState.h"
#include "CovertCounterOps/Game/Interfaces/LoadoutItem.h"
#include "Interfaces/AIUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ASetupPhaseCharacter::ASetupPhaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASetupPhaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	_CurrentBuildPoints = _TotalBuildPoints;
}

void ASetupPhaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASetupPhaseCharacter, _CurrentBuildPoints);
}

void ASetupPhaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASetupPhaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASetupPhaseCharacter::TrySetDesiredSpawnPoint(FVector SpawnLocation)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || gameState->GetRoundState() != ERoundState::SETUP_PHASE)
	{
		return;
	}
	
	PredictedSetDesiredSpawnPoint(SpawnLocation);
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		Server_SetDesiredSpawnPoint(SpawnLocation);
	}
}

void ASetupPhaseCharacter::PredictedSetDesiredSpawnPoint(FVector SpawnLocation)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || gameState->GetRoundState() != ERoundState::SETUP_PHASE)
	{
		return;
	}
	
	if (IsValidSpawnLocation(static_cast<ETeam>(_TeamNumber), SpawnLocation))
	{
		if (AController* controller = GetController())
		{
			if (AMainPlayerState* playerState = controller->GetPlayerState<AMainPlayerState>())
			{
				playerState->SetDesiredSpawnPoint(SpawnLocation);
			}
		}
	}
}

void ASetupPhaseCharacter::Server_SetDesiredSpawnPoint_Implementation(FVector_NetQuantize SpawnLocation)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || gameState->GetRoundState() != ERoundState::SETUP_PHASE)
	{
		return;
	}
	PredictedSetDesiredSpawnPoint(SpawnLocation);
}

bool ASetupPhaseCharacter::IsValidSpawnLocation_Implementation(ETeam ValidForTeam, FVector SpawnLocation)
{
	return true;
}

void ASetupPhaseCharacter::Server_RequestSpawnActor_Implementation(UClass* ActorClass, FVector_NetQuantize SpawnLocation, FVector_NetQuantize SpawnNormal)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || gameState->GetRoundState() != ERoundState::SETUP_PHASE || _CurrentBuildPoints == 0)
	{
		return;
	}
	
	ETeam team = static_cast<ETeam>(_TeamNumber);
	if (IsValidSpawnLocation(team, SpawnLocation))
	{
		FRotator rotator = UKismetMathLibrary::MakeRotFromX(SpawnNormal);
		AActor* spawnedActor = GetWorld()->SpawnActor(ActorClass, &SpawnLocation, &rotator);

		if (!IsValid(spawnedActor) || !spawnedActor->Implements<ULoadoutItem>())
		{
			return;
		}

		int buildCost = ILoadoutItem::Execute_GetLoadoutItemData(spawnedActor)->_buildCost;
		if (_CurrentBuildPoints - buildCost <= 0)
		{
			if (APawn* pawn = Cast<APawn>(spawnedActor))
			{
				if (AController* controller = pawn->GetController())
				{
					controller->UnPossess();
					controller->Destroy();
				}
			}
			
			spawnedActor->Destroy();
			return;
		}
		else
		{
			_CurrentBuildPoints -= buildCost;
		}
		
		if (spawnedActor->Implements<UCommonUtility>())
		{
			ICommonUtility::Execute_SetTeam(spawnedActor, static_cast<int32>(team));
		}

		if (spawnedActor->Implements<UAIUtility>())
		{
			IAIUtility::Execute_InitialiseAIPawn(spawnedActor);
		}

		A1VXGameMode* gameMode = Cast<A1VXGameMode>(GetWorld()->GetAuthGameMode());
		if (IsValid(gameMode))
		{
			gameMode->AddSpawnedRoundActor(spawnedActor);
		}
	}
}

void ASetupPhaseCharacter::Server_RequestDestroyActor_Implementation(AActor* ActorToDestroy)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState) || gameState->GetRoundState() != ERoundState::SETUP_PHASE)
	{
		return;
	}
	A1VXGameMode* gameMode = Cast<A1VXGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(gameMode))
	{
		if (gameMode->IsSpawnedRoundActor(ActorToDestroy))
		{
			if (!IsValid(ActorToDestroy) || !ActorToDestroy->Implements<ULoadoutItem>())
			{
				return;
			}
			
			gameMode->RemoveSpawnedRoundActor(ActorToDestroy);
			int buildCost = ILoadoutItem::Execute_GetLoadoutItemData(ActorToDestroy)->_buildCost;
			
			if (APawn* pawn = Cast<APawn>(ActorToDestroy))
			{
				if (AController* controller = pawn->GetController())
				{
					controller->UnPossess();
					controller->Destroy();
				}
			}
			
			ActorToDestroy->Destroy();
			_CurrentBuildPoints += buildCost;
		}
	}
}

void ASetupPhaseCharacter::DestroySelectedActors()
{
	for (auto actor : _SelectedActors)
	{
		Server_RequestDestroyActor(actor);
	}
	_SelectedActors.Empty();
}

