// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Activatable.h"
#include "Objective.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveCompleted, AObjective*, CompletedObjective);

UCLASS()
class COVERTCOUNTEROPS_API AObjective : public AActivatable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObjective();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetObjectiveCompleted();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetObjective();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ShowObjectiveUI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HideObjectiveUI();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnObjectiveCompleted OnObjectiveCompletedDelegate;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

};
