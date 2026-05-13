// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "UE5Coro.h"
#include "CovertCounterOpsWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class COVERTCOUNTEROPS_API ACovertCounterOpsWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ACovertCounterOpsWorldSettings* GetWorldSettings() { return _WorldSettings; }
	
	UFUNCTION(BlueprintCallable)
	int GetNumFloors() { return _StreamedLevels.Num(); }
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	FTransform GetDefenderSpawnTransform() const { return IsValid(_DefenderSpawnLocation) ? _DefenderSpawnLocation->GetActorTransform() : FTransform(); }

	UFUNCTION(BlueprintCallable)
	void SetFloorVisibility(int FloorNumber, bool IsHidden);

	UFUNCTION(BlueprintCallable)
	void ShowAllFloors();

	UFUNCTION(BlueprintCallable)
	void ViewPreviousFloor();
	
	UFUNCTION(BlueprintCallable)
	void ViewNextFloor();
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor> _DefenderSpawnLocation;

	TArray<ULevelStreaming*> _StreamedLevels;

	UPROPERTY(BlueprintReadOnly)
	int _CurrentViewedFloor = 0;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<FString> _SublevelByFloors;

	static TObjectPtr<ACovertCounterOpsWorldSettings> _WorldSettings;
};
