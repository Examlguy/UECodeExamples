// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameConfigurationInstanceSubsystem.generated.h"

#define NULL_SEED -1

UENUM(BlueprintType)
enum class EDifficultyConfig : uint8
{
	EASY = 0,
	NORMAL = 1,
	HARD = 2,
	EXTREME = 3
};

UENUM(BlueprintType)
enum class EPlayMode : uint8
{
	STANDARD = 0,
	ENDLESS = 1,
	NOCLIP = 2,
	PEACEFUL = 3,
	GALLERY = 4,
	RACE = 5
};

UCLASS()
class WHEREISMARCO_API UGameConfigurationInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void WaitForGameShadersBuilt();
	
	UFUNCTION(BlueprintCallable)
	void ClearSeed() { _gameSeed = NULL_SEED; }
	
	UFUNCTION(BlueprintCallable)
	void SetGameSeed(int seed) { _gameSeed = seed; }
	
	UFUNCTION(BlueprintCallable)
	int GetGameSeed() const { return _gameSeed == NULL_SEED ? FMath::Rand() : _gameSeed; }

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(EDifficultyConfig difficulty) { _difficulty = difficulty; }
	
	UFUNCTION(BlueprintCallable)
	EDifficultyConfig GetDifficulty() const { return _difficulty; }

	UFUNCTION(BlueprintCallable)
	void SetPlayMode(EPlayMode play_mode) { _playMode = play_mode; }

	UFUNCTION(BlueprintCallable)
	EPlayMode GetPlayMode() const { return _playMode; }
	
private:
	UPROPERTY()
	int _gameSeed = NULL_SEED;
	
	UPROPERTY()
	EDifficultyConfig _difficulty = EDifficultyConfig::NORMAL;

	UPROPERTY()
	EPlayMode _playMode = EPlayMode::STANDARD;
};
