// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "WhereIsMarco/WhereIsMarcoPlayerState.h"
#include "SpectatorCharacter.generated.h"

UCLASS()
class WHEREISMARCO_API ASpectatorCharacter : public ASpectatorPawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	ASpectatorCharacter();

	UFUNCTION()
	void FindNextSpectatableActor();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerCharacterStateChanged(AWhereIsMarcoPlayerState* player_state, EPlayerCharacterState new_state);
	
	UPROPERTY()
	TObjectPtr<AWhereIsMarcoPlayerState> _currentSpectatingPlayerState;
	
	UPROPERTY()
	TObjectPtr<USceneComponent> _currentSpectatingComponent;
};
