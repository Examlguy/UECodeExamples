#include "MainPlayerController.h"

#include "AdvancedSightTeamComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "CovertCounterOps/Characters/RTSCharacter.h"
#include "CovertCounterOps/Characters/Interfaces/Possessable.h"
#include "CovertCounterOps/Game/A1VXGameMode.h"
#include "CovertCounterOps/Game/A1VXGameState.h"
#include "CovertCounterOps/Game/Interfaces/Visibility.h"
#include "Net/UnrealNetwork.h"

AMainPlayerController::AMainPlayerController()
{
}

void AMainPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainPlayerController, _MainControlledPawn);
}

float AMainPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PlayerControllerMappingContext, 999);
	}
	if (A1VXGameMode* gameMode = GetWorld()->GetAuthGameMode<A1VXGameMode>())
	{
		gameMode->BindToPlayerState(GetPlayerState<AMainPlayerState>());
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTimeSync(DeltaTime);
}

void AMainPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

int32 AMainPlayerController::GetControllerTeam() const
{
	if (IsValid(GetPawn()))
	{
		UAdvancedSightTeamComponent* teamComponent = GetPawn()->FindComponentByClass<UAdvancedSightTeamComponent>();
		return IsValid(teamComponent) ? teamComponent->TeamID : static_cast<int32>(ETeam::UNASSIGNED);
	}

	return static_cast<int32>(ETeam::UNASSIGNED);
}

void AMainPlayerController::SetupPossessableData(APawn* InPawn)
{
	RemovePossessableData();
	if (InPawn->Implements<UPossessable>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (UInputMappingContext* inputMapping = IPossessable::Execute_GetControlMapping(InPawn))
			{
				Subsystem->AddMappingContext(inputMapping, 0);
				_MappingContextsInUse.Add(inputMapping);
			}
		}

		if (TSubclassOf<UUserWidget> pawnHUD = IPossessable::Execute_GetPossessableHUD(InPawn))
		{
			_CurrentControlledHUD = CreateWidget(this, pawnHUD);
			if (IsValid(_CurrentControlledHUD))
			{
				_CurrentControlledHUD->AddToViewport();
			}
		}
	}
	
	if (ARTSCharacter* rtsCharacter = Cast<ARTSCharacter>(GetPawn()))
	{
		ShowWhatRTSCantSee(false);
	}
	else
	{
		ShowWhatRTSCantSee(true);
	}
}

void AMainPlayerController::RemovePossessableData()
{
	for (UInputMappingContext* mappingContext : _MappingContextsInUse)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(mappingContext);
		}
	}
	_MappingContextsInUse.Empty();

	if (IsValid(_CurrentControlledHUD))
	{
		_CurrentControlledHUD->RemoveFromParent();
	}
}

void AMainPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	SetupPossessableData(InPawn);
}

void AMainPlayerController::ShowWhatRTSCantSee(bool IsVisible)
{
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return;
	}
	
	for (auto player : gameState->GetAttackerPlayers())
	{
		APawn* playerPawn = player->GetPawn();
		if (IsValid(playerPawn) && player->GetPawn()->Implements<UVisibility>())
		{
			IVisibility::Execute_SetVisibility(player->GetPawn(), IsVisible);
		}
	}
}

void AMainPlayerController::SetMainControlledPawn(APawn* InPawn)
{
	_MainControlledPawn = InPawn;
}

void AMainPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		Server_RequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AMainPlayerController::Server_RequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	Client_ReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AMainPlayerController::Client_ReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	_SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AMainPlayerController::Client_RequestPossessPawn_Implementation(APawn* InPawn)
{
	Server_RequestPossessPawn(InPawn);
}

void AMainPlayerController::Server_RequestPossessPawn_Implementation(APawn* InPawn)
{
	if (IsValid(InPawn->GetController<AMainPlayerController>()))
	{
		return;
	}

	if (!InPawn->Implements<UPossessable>())
	{
		return;
	}

	if (IPossessable::Execute_CanBePossessedBy(InPawn, this))
	{
		bool HasDefaultAIController = IsValid(GetPawn()->AIControllerClass);
		AController* currentPawnDefaultController = HasDefaultAIController ? Cast<AController>(GetWorld()->SpawnActor(GetPawn()->AIControllerClass)) : nullptr;
		APawn* oldPawn = GetPawn();
		
		UnPossess();
		
		if (HasDefaultAIController && IsValid(currentPawnDefaultController))
		{
			if (_MainControlledPawn == oldPawn)
			{
				currentPawnDefaultController->Destroy();
			}
			else
			{
				currentPawnDefaultController->Possess(oldPawn);
			}
		}
		
		if (AController* newController = InPawn->Controller)
		{
			newController->UnPossess();
			newController->Destroy();
		}
		
		Possess(InPawn);
	}
}
