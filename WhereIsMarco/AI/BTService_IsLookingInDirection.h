// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsLookingInDirection.generated.h"

/**
 * 
 */
UCLASS()
class WHEREISMARCO_API UBTService_IsLookingInDirection : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_IsLookingInDirection();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
