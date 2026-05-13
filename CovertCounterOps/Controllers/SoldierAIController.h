// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedSightComponent.h"
#include "GMCAIController.h"
#include "HTN.h"
#include "HTNComponent.h"
#include "SoldierAIController.generated.h"

UENUM(Blueprintable)
enum class ESpotState : uint8
{
	NONE,
	HEARD,
	FORGOT,
	LOST,
	SPOTTED,
	PERCEIVED
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTargetPerceptionState, APawn*, SeenBy, APawn*, Target, ESpotState, SpotState);

UCLASS()
class COVERTCOUNTEROPS_API ASoldierAIController : public AGMC_AIController
{
	GENERATED_BODY()

public:
	ASoldierAIController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetFocalPointOnActor(const AActor *Actor) const override;
	virtual bool LineOfSightTo(const AActor* Other, FVector ViewPoint = FVector(ForceInit), bool bAlternateChecks = false) const override;
	virtual void UpdateControlRotation(float DeltaTime, bool ShouldUpdatePawn = true) override;

	UFUNCTION(BlueprintCallable)
	float DistanceToTarget(AActor* InTarget);
	
	UFUNCTION(BlueprintCallable)
	void ProcessVisionedTarget(AActor* InTarget, ESpotState SpotState);

	UFUNCTION(BlueprintCallable)
	void GetNextPriorityTarget();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnTargetPerceptionState OnTargetPerceptionStateDelegate;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAdvancedSightComponent> _AdvancedSightComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHTNComponent> _HTNComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHTN> _BehaviourBrain;
};
