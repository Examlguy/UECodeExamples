// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_HasReachedPosition.generated.h"

/**
 * 
 */
UCLASS()
class WHEREISMARCO_API UBTDecorator_HasReachedPosition : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTDecorator_HasReachedPosition();
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere, Category = Condition, meta = (ClampMin = "0.0"))
	float _acceptableRadius;
};
