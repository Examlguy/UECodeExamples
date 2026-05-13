// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "GhostGuessrWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class GHOSTGUESSR_API AGhostGuessrWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetPlayerSpawnLocations() const { return _PlayerSpawns; }

private:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AActor>> _PlayerSpawns;

};
