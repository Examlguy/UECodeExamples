// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MainPlayerState.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "CovertCounterOps/Controllers/SoldierAIController.h"
#include "GameFramework/GameMode.h"
#include "A1VXGameMode.generated.h"

enum class ERoundState : uint8;

UCLASS(minimalapi)
class A1VXGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	A1VXGameMode();

	virtual void PreInitializeComponents() override;
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void RestartRound();

	UFUNCTION(BlueprintCallable)
	void StartSetupPhase();

	UFUNCTION(BlueprintCallable)
	void EndSetupPhase();

	UFUNCTION(BlueprintCallable)
	void StartRound();
	
	UFUNCTION(BlueprintCallable)
	void EndRound();

	UFUNCTION()
	void HandleStartingSpawns();
	
	UFUNCTION()
	void BindToPlayerState(AMainPlayerState* MainPlayerState);

	UFUNCTION()
	void OnPlayerPlayStateChanged(AMainPlayerState* MainPlayerState, EPlayerPlayState NewState);

	UFUNCTION()
	void AllValidPlayersReady();

	UFUNCTION()
	void OnRoundStateChanged(ERoundState NewRoundState);
	
	UFUNCTION()
	void KillPlayer(AMainPlayerController* PlayerController);
	
	UFUNCTION()
	APawn* RespawnPlayerAsClass(APlayerState* PlayerState, UClass* NewClass, FVector SpawnLocation, FRotator SpawnRotation, bool DestroyOldPawn = true);

	UFUNCTION()
	void OnPlayerSpottedEvent(APawn* SeenBy, APawn* Target, ESpotState SpotState);

	UFUNCTION(BlueprintCallable)
	void AddSpawnedRoundActor(AActor* SpawnedActor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsSpawnedRoundActor(AActor* SpawnedActor);
	
	UFUNCTION(BlueprintCallable)
	void RemoveSpawnedRoundActor(AActor* ActorToRemove);
	
	UFUNCTION(BlueprintCallable)
	void CleanupSpawnedRoundActors();
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<APawn*> _AllDefenderSoldiers;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<APawn*> _AllDefenderCharacters;

	UPROPERTY(BlueprintReadOnly)
	TArray<APawn*> _AllAttackerCharacters;

	UPROPERTY()
	TArray<AActor*> _SpawnedRoundActors;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> _SetupCharacter;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> _DefenderCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> _AttackerCharacter;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> _SoldierCharacter;
};



