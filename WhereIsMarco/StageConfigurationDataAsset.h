// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameConfigurationInstanceSubsystem.h"
#include "Engine/DataAsset.h"
#include "StageConfigurationDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FPlayModeDataSet
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPlayMode playMode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> clueTexts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UClass>> stageObjectsToSpawn;
};

UCLASS()
class WHEREISMARCO_API UStageConfigurationDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayModeDataSet> _playModeSetup;
};
