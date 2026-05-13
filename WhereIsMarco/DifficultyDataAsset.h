// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/MonsterBaseCharacter.h"
#include "Engine/DataAsset.h"
#include "DifficultyDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FClueDifficulty
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-90.0", ClampMax = "0.0", UIMin = "-90.0", UIMax = "0.0"))
	int _marcoLookDownRange = 0;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
	int _marcoLookUpRange = 0;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AMonsterBaseCharacter>> _monstersToSpawn;
};

UCLASS()
class WHEREISMARCO_API UDifficultyDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UDifficultyDataAsset();
	
	UFUNCTION(BlueprintCallable)
	TArray<TSubclassOf<AMonsterBaseCharacter>> GetMonstersToSpawnAtClueNumber(int clue_number);

	UFUNCTION(BlueprintCallable)
	int GetNumberOfScreenshots() const { return _difficultyAtClue.Num(); }
	
	UFUNCTION(BlueprintCallable)
	FClueDifficulty GetClueDifficultyAtClueNumber(int clue_number);
	
private:
	UPROPERTY(EditAnywhere)
	TArray<FClueDifficulty> _difficultyAtClue;

	UPROPERTY()
	int _numberOfScreenshots = 1;
	
	UPROPERTY()
	TSet<TSubclassOf<AMonsterBaseCharacter>> _spawnableMonsters;
};
