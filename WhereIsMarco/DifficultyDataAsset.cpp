#include "DifficultyDataAsset.h"

UDifficultyDataAsset::UDifficultyDataAsset()
{
	for (FClueDifficulty difficultyStage : _difficultyAtClue)
	{
		for (TSubclassOf<AMonsterBaseCharacter> monster : difficultyStage._monstersToSpawn)
		{
			if (!_spawnableMonsters.Find(monster))
			{
				_spawnableMonsters.Add(monster);
			}
		}
	}
}

TArray<TSubclassOf<AMonsterBaseCharacter>> UDifficultyDataAsset::GetMonstersToSpawnAtClueNumber(int clue_number)
{
	if (clue_number >= 0 && clue_number < _difficultyAtClue.Num())
	{
		return _difficultyAtClue[clue_number]._monstersToSpawn;
	}

	if (_difficultyAtClue.Num() != 0)
	{
		return _difficultyAtClue[_difficultyAtClue.Num() - 1]._monstersToSpawn;
	}

	TArray<TSubclassOf<AMonsterBaseCharacter>> monsters;
	for (int i = 0; i < FMath::RandRange(1, 8); i++)
	{
		monsters.Add(_spawnableMonsters.Get(FSetElementId::FromInteger(FMath::RandRange(1, 8))));
	}

	return monsters;
}

FClueDifficulty UDifficultyDataAsset::GetClueDifficultyAtClueNumber(int clue_number)
{
	if (clue_number >= 0 && clue_number < _difficultyAtClue.Num())
	{
		return _difficultyAtClue[clue_number];
	}

	if (_difficultyAtClue.Num() != 0)
	{
		return _difficultyAtClue[_difficultyAtClue.Num() - 1];
	}
	
	return FClueDifficulty();
}
