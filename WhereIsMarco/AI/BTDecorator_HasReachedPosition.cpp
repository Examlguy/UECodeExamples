#include "BTDecorator_HasReachedPosition.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTDecorator_HasReachedPosition::UBTDecorator_HasReachedPosition()
{
	_acceptableRadius = 50.0f;
}

bool UBTDecorator_HasReachedPosition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	UBlackboardComponent* blackboardComponent = OwnerComp.GetBlackboardComponent();
	if (IsValid(blackboardComponent))
	{
		FVector targetLocation = blackboardComponent->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		return (targetLocation - OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation()).Length() <= _acceptableRadius;
	}

	return false;
}
