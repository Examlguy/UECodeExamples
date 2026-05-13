// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AssignBool.generated.h"

UCLASS()
class WHEREISMARCO_API UBTTask_AssignBool : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_AssignBool();
protected:
	virtual EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
