// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VisibilityPointsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UVisibilityPointsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTGUESSR_API IVisibilityPointsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<USceneComponent*> GetVisibilityPoints();
	virtual TArray<USceneComponent*> GetVisibilityPoints_Implementation();
};
