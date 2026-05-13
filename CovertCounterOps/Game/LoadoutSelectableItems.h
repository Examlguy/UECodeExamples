// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadoutItemData.h"
#include "Engine/DataAsset.h"
#include "LoadoutSelectableItems.generated.h"

UCLASS()
class COVERTCOUNTEROPS_API ULoadoutSelectableItems : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<ULoadoutItemData>> _SelectableLoadoutItemData;
};
