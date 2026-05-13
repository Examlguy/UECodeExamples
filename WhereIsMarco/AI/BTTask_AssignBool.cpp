
#include "BTTask_AssignBool.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AssignBool::UBTTask_AssignBool()
{
	NodeName = TEXT("Assign Bool Value");
}

EBTNodeResult::Type UBTTask_AssignBool::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);

	return EBTNodeResult::Succeeded;
}
