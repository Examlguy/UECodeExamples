// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorCharacter.h"

#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "CovertCounterOps/Game/MainPlayerState.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ASpectatorCharacter::ASpectatorCharacter()
{

}

// Called when the game starts or when spawned
void ASpectatorCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpectatorCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	if (UKismetSystemLibrary::IsServer(GetWorld()) && IsValid(GetController<AMainPlayerController>()))
	{
		AMainPlayerController* playerController = GetController<AMainPlayerController>();
		AMainPlayerState* playerState = playerController->GetPlayerState<AMainPlayerState>();
		if (IsValid(playerState))
		{
			playerState->SetPlayerPlayState(EPlayerPlayState::SPECTATING);
		}
	}
}

void ASpectatorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpectatorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

