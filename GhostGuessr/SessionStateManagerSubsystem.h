// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SessionStateManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTGUESSR_API USessionStateManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "WorldStateManager")
	int32 GetSessionDeathCount() { return _SessionDeathCount; }
	
	UFUNCTION(BlueprintCallable, Category = "WorldStateManager")
	void IncreaseSessionDeathCount() { _SessionDeathCount++; }
private:
	int32 _SessionDeathCount = 0;
};
