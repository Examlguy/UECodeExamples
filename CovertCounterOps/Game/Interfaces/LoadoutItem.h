// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CovertCounterOps/Game/LoadoutItemData.h"
#include "UObject/Interface.h"
#include "LoadoutItem.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULoadoutItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COVERTCOUNTEROPS_API ILoadoutItem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	ULoadoutItemData* GetLoadoutItemData();
	virtual ULoadoutItemData* GetLoadoutItemData_Implementation();
};
