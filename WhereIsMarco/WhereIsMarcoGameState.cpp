#include "WhereIsMarcoGameState.h"

#include "Controller/Player/MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "World/Interactable/Activatable.h"

void AWhereIsMarcoGameState::Multicast_GameStageUpdated_Implementation(uint8 new_gamestage)
{
	_gameStage = new_gamestage;
	OnGameStageChanged.Broadcast(new_gamestage);
}

void AWhereIsMarcoGameState::Multicast_RoundStateUpdated_Implementation(ERoundState new_roundstate)
{
	_gameRoundState = new_roundstate;
	OnRoundStateChanged.Broadcast(new_roundstate);
}

void AWhereIsMarcoGameState::BeginPlay()
{
	Super::BeginPlay();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), ToggleableLight.GetTag().GetTagName(), _toggleableLights);
}

void AWhereIsMarcoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWhereIsMarcoGameState, _numberOfMarcoLocationsExpected);
	DOREPLIFETIME(AWhereIsMarcoGameState, _currentMarcoLocationClue);
}

void AWhereIsMarcoGameState::Multicast_KillPlayer_Implementation(AMonsterBaseCharacter* killer_character,
	APlayerCharacter* playerCharacter, float animation_duration)
{
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0) == playerCharacter->GetController())
	{
		if (IsValid(killer_character))
		{
			if (AMainPlayerController* playerController = Cast<AMainPlayerController>(playerCharacter->GetController()))
			{
				playerController->LookAtSceneComponent(killer_character->GetScareLookAtLocation(), animation_duration);
			}
		}
	}
}

void AWhereIsMarcoGameState::Multicast_ToggleLights_Implementation(bool toggle_on)
{
	for (AActor* actor : _toggleableLights)
	{
		AActivatable* activatable = Cast<AActivatable>(actor);
		if (IsValid(activatable))
		{
			activatable->ToggleActivate(toggle_on);
		}
	}

	ToggleAllLights(toggle_on);
}
void AWhereIsMarcoGameState::FoundMarcoLocation(APawn* discovering_player)
{
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		AWhereIsMarcoPlayerState* playerState = Cast<AWhereIsMarcoPlayerState>(discovering_player->GetPlayerState());
		if (IsValid(playerState))
		{
			if (_currentMarcoLocationClue < _marcoLocationsReceived.Num() - 1)
			{
				_currentMarcoLocationClue++;
				Multicast_ShowMarcoLocationScreenshot(_currentMarcoLocationClue);
				playerState->SetScore(playerState->GetScore() + 100);
			}
			else
			{
				playerState->SetPlayerWonThisRound(true);
				playerState->SetScore(playerState->GetScore() + 1000);
				OnPlayerWon.Broadcast(discovering_player);
			}
		}
	}
}

void AWhereIsMarcoGameState::SetPlayersAllowedToMove(bool can_move)
{
	for (APlayerState* playerState : PlayerArray)
	{
		APlayerCharacter* player = Cast<APlayerCharacter>(playerState->GetPawn());
		if (IsValid(player))
		{
			player->SetMovementEnabled(can_move);
		}
	}
}

void AWhereIsMarcoGameState::Multicast_SetPlayersAllowedToMove_Implementation(bool can_move)
{
	SetPlayersAllowedToMove(can_move);
}

void AWhereIsMarcoGameState::Multicast_AllPlayersReady_Implementation()
{
	OnAllPlayersReadyDelegate.Broadcast();
}

void AWhereIsMarcoGameState::Multicast_ShowMarcoLocationScreenshot_Implementation(int key)
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		OnShowingMarcoScreenshot.Broadcast(key);
		player->ShowMarcoLocationScreenshot(key);
	}
}

void AWhereIsMarcoGameState::Multicast_RestartMarcoLocationTextures_Implementation(int num_screenshots_to_expect)
{
	_marcoLocationsReceived.Empty();
	_numberOfMarcoLocationsExpected = num_screenshots_to_expect;
	_currentMarcoLocationClue = START_MARCO_LOCATION;
}

void AWhereIsMarcoGameState::AddMarcoLocationScreenshot_Implementation(int key, UTexture2D* texture, FVector location)
{
	_marcoLocationsReceived.Add(key,FMarcoLocation(texture, location));
}
