// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GMCAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "WhereIsMarco/WhereIsMarcoGameMode.h"
#include "WhereIsMarco/Character/Monster/MonsterBaseCharacter.h"
#include "MonsterBaseController.generated.h"

UCLASS()
class WHEREISMARCO_API AMonsterBaseController : public AGMC_AIController
{
	GENERATED_BODY()
public:
	AMonsterBaseController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void PlaceCheckedAreaMarker(FVector location);

	UFUNCTION(BlueprintCallable)
	void ClearCheckedAreaMarkers();

	UFUNCTION(BlueprintCallable)
	void AttachLastTargetPosition(APawn* target_pawn);

	UFUNCTION(BlueprintCallable)
	void DetachLastTargetPosition();
	
	UFUNCTION(BlueprintCallable)
	void ReturnLastTargetPosition();
	
	UFUNCTION(BlueprintCallable)
	TArray<AActor*> GetCheckedAreaMarkers() const { return _checkedAreaMarkers; }

	UFUNCTION(BlueprintCallable)
	AActor* GetTargetLastPositionMarker() const { return _targetLastPositionMarker; }
	
	UFUNCTION(BlueprintCallable)
	virtual bool DistanceFromActorInRange(AActor* target_actor, float within_distance);
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanPathToActor(AActor* target_actor);

	UFUNCTION(BlueprintCallable)
	virtual void TeleportToLocation(FVector new_location);
	
	UFUNCTION()
	void OnRoundStateChanged(ERoundState new_roundstate);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetSpottedEvent(APawn* target_actor, bool spotted);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCloseProximityPlayerChanged(APawn* target_actor, bool spotted);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCloseVisiblePlayerChanged(APawn* target_actor, bool spotted);

	UFUNCTION(BlueprintCallable)
	APawn* GetCurrentTarget() const { return _currentTarget; }
	
	UFUNCTION(BlueprintCallable)
	float GetChaseRange() const { return _targetChaseRange; }

protected:
	UFUNCTION()
	void ProcessTargets();
	
	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnPerception(AActor* actor_detected, FAIStimulus stimulus);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& other_actor) const override;
	
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetCheckedAreaMarkers)
	TArray<TObjectPtr<AActor>> _checkedAreaMarkers;
	int _currentCheckedMarker = 0;
	
	UPROPERTY(BlueprintReadOnly, BlueprintGetter=GetTargetLastPositionMarker)
	TObjectPtr<AActor> _targetLastPositionMarker;

	UPROPERTY(BlueprintReadOnly)
	bool _isLastPositionMarkerAttached = false;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> _checkedAreaMarkerClass;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> _targetLastPositionMarkerClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float _targetChaseRange = 500;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 _maxCheckedMarkers = 8;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> _closestProximityPlayer;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<APawn> _closestVisiblePlayer;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APawn> _currentTarget;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<TObjectPtr<APlayerState>, bool> _playerSpotStates;
	
	UPROPERTY(BlueprintReadOnly)
	FVector _lastTargetHeardLocation;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBehaviorTree> _monsterBehaviour;

	UPROPERTY()
	TObjectPtr<AMonsterBaseCharacter> _monsterCharacter;
};
