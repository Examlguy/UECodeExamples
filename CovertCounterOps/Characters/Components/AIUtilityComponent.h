// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CovertCounterOps/Controllers/SoldierAIController.h"
#include "CovertCounterOps/Game/A1VXGameState.h"
#include "AIUtilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPatrolPointsUpdatedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActorSelected, AActor*, SelectedActor, bool, IsSelected);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COVERTCOUNTEROPS_API UAIUtilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties 
	UAIUtilityComponent();

	UFUNCTION()
	APawn* GetCurrentTarget() const { return _CurrentTarget.Get(); }
	
	UFUNCTION(BlueprintCallable)
	void SelectActor(bool IsSelected);
	
	void SetFocusRequest(FVector FocusLocation, bool HasFocusLocation);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void FindAndSetNearestPatrolPointFirst();
	
	UFUNCTION(BlueprintCallable)
	void AddPatrolPoint(FVector& PatrolPoint, bool Additive);
	
	UFUNCTION(BlueprintCallable)
	FVector GetNextPatrolPoint();

	UFUNCTION(BlueprintCallable)
	void ClearPatrolPoints();

	UFUNCTION()
	void OnOverallAlertStateChanged(EAlertState NewOverallAlertState);

	UFUNCTION()
	void OnTargetPerceptionState(APawn* SeenBy, APawn* Target, ESpotState SpotState);

	UFUNCTION()
	void OnRep_AlertState();

	UFUNCTION(BlueprintImplementableEvent)
	void AlertStateChangedEvent();
	
	UFUNCTION()
	void OnRep_SpotState();

	UFUNCTION(BlueprintImplementableEvent)
	void SpotStateChangedEvent();
	
	UPROPERTY(BlueprintAssignable)
	FOnPatrolPointsUpdatedDelegate OnPatrolPointsUpdatedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnActorSelected OnActorSelectedDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	TSoftObjectPtr<APawn> _CurrentTarget;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> _PatrolPoints = {};

	UPROPERTY()
	int _PatrolStartIndex = 0;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AlertState)
	EAlertState _AlertState = EAlertState::LOW;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_SpotState)
	ESpotState _SpotState = ESpotState::NONE;

	UPROPERTY(BlueprintReadOnly)
	FVector _FocusLocation;
	
	UPROPERTY(BlueprintReadWrite)
	bool _HasFocusLocation;
};
