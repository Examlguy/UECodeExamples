// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsTargetClose.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "WhereIsMarco/Controller/Monster/MonsterBaseController.h"

UBTService_IsTargetClose::UBTService_IsTargetClose()
{
}

void UBTService_IsTargetClose::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (AMonsterBaseController* controller = Cast<AMonsterBaseController>(OwnerComp.GetAIOwner()))
	{
		APawn* currentTarget = controller->GetCurrentTarget();
		if (IsValid(currentTarget) && (controller->GetPawn()->GetActorLocation() - currentTarget->GetActorLocation()).Length() <= controller->GetChaseRange())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
			return;
		}
	}

	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
}
