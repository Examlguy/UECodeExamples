// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CovertCounterOps/Items/Objective.h"
#include "UObject/Object.h"
#include "ObjectiveSet.generated.h"

class AActivatable;

UCLASS(Blueprintable)
class COVERTCOUNTEROPS_API AObjectiveSet : public AActor
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	AObjective* GetSelectedObjective() const { return _SelectedObjective; }
	
	UFUNCTION()
	void GenerateObjective();
	
	UFUNCTION()
	void ResetObjectives();

	UFUNCTION()
	void OnRep_SelectedObjective();
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_SelectedObjective, BlueprintReadOnly)
	TObjectPtr<AObjective> _SelectedObjective;
	
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AObjective>> _PotentialObjectiveItems;
};
