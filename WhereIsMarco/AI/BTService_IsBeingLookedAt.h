// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "UE5Coro/Coroutine.h"
#include "BTService_IsBeingLookedAt.generated.h"

/**
 * 
 */
UCLASS()
class WHEREISMARCO_API UBTService_IsBeingLookedAt : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_IsBeingLookedAt();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UE5Coro::TCoroutine<> IsBeingLookedAt(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
};
