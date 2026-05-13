// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LoadoutItemData.generated.h"

/**
 * 
 */
UCLASS()
class COVERTCOUNTEROPS_API ULoadoutItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> _buildItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText _itemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int _buildCost = 0;
};
