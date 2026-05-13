#pragma once

#include "CoreMinimal.h"
#include "DifficultyDataAsset.h"
#include "WhereIsMarcoPlayerState.h"
#include "Character/Monster/MarcoCharacter.h"
#include "Character/Player/PlayerCharacter.h"
#include "GameFramework/GameMode.h"
#include "WhereIsMarcoGameMode.generated.h"

UENUM(BlueprintType)
enum class ERoundState : uint8
{
	PREGINNING = 0,
	BEGINNING = 1,
	INPROGRESS = 2,
	FINISHED = 3
};

UCLASS()
class WHEREISMARCO_API AWhereIsMarcoGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AWhereIsMarcoGameMode();

	virtual void StartPlay() override;
	
	UFUNCTION(BlueprintCallable)
	bool TryFinishStage();

	UFUNCTION(BlueprintCallable)
	void ForceRestartStage(bool force_respawn, bool stop_spawn_movement);

	UFUNCTION(BlueprintCallable)
	bool TryStartStage();
	
	UFUNCTION(BlueprintCallable)
	bool TryNextStage();

	UFUNCTION(BlueprintCallable)
	bool TryKillPlayer(AMonsterBaseCharacter* killer_character = nullptr, APlayerCharacter* player_character = nullptr, float animation_duration = 0.0f);

	UFUNCTION(BlueprintCallable)
	void RespawnMonsters();

	UFUNCTION(BlueprintCallable)
	bool ArePlayersStillAlive() const;

	UFUNCTION(BlueprintCallable)
	bool ArePlayersStillBusy() const;
	
	UFUNCTION(BlueprintCallable)
	bool AreObjectivesCompleted() const;
	
	UFUNCTION()
	void ReceiveReadyPlayerState(AWhereIsMarcoPlayerState* player_state);
	
	UFUNCTION()
	void OnPlayerReadyChanged(bool new_state);
	
	UFUNCTION()
	void OnPlayerStateChanged(AWhereIsMarcoPlayerState* player_state, EPlayerCharacterState new_state);

	UFUNCTION()
	void OnPlayerWon(APawn* winning_player);
	
	UFUNCTION(BlueprintCallable)
	uint8 GetGameStage() const { return _gameStage; }
	
	UFUNCTION(BlueprintCallable)
	ERoundState GetRoundState() const { return _roundState; }

	UFUNCTION(BlueprintCallable)
	AActor* GetMarcoActor() const { return _marcoCharacter; }

	UFUNCTION(BlueprintCallable)
	int GetNumSpawnedMonsters() const { return _spawnedMonsters.Num(); }
	
	UFUNCTION(BlueprintCallable)
	UDifficultyDataAsset* GetDifficultyData() const { return _currentDifficulty; }

	
protected:
	UFUNCTION(BlueprintCallable)
	void RespawnPlayers(bool force_respawn = true);
	
	UFUNCTION(BlueprintCallable)
	void RespawnPlayerAsClass(AWhereIsMarcoPlayerState* player, UClass* new_class, FVector spawn_location, FRotator spawn_rotation);
	
	UFUNCTION(BlueprintCallable)
	void SetGameStage(uint8 new_game_stage);

	UFUNCTION(BlueprintCallable)
	void SetRoundState(ERoundState new_round_state);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMarcoCharacter> _marcoCharacterClass;
	
private:
	UPROPERTY(EditAnywhere, BlueprintSetter=SetGameStage)
	uint8 _gameStage = 0;

	UPROPERTY(EditAnywhere, BlueprintSetter=SetRoundState)
	ERoundState _roundState = ERoundState::PREGINNING;

	UPROPERTY()
	TObjectPtr<UDifficultyDataAsset> _currentDifficulty;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDifficultyDataAsset> _easyDifficulty;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDifficultyDataAsset> _normalDifficulty;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDifficultyDataAsset> _hardDifficulty;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDifficultyDataAsset> _extremeDifficulty;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> _objectiveActivatables;

	UPROPERTY()
	TArray<TObjectPtr<AMonsterBaseCharacter>> _spawnedMonsters;
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> _spawnLocations;
	
	UPROPERTY()
	TObjectPtr<AMarcoCharacter> _marcoCharacter;

	UPROPERTY()
	bool _playersMovementDisabled;
};
