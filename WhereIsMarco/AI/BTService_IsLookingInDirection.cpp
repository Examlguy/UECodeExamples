#include "BTService_IsLookingInDirection.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/Controller/Player/MainPlayerController.h"
#include "WhereIsMarco/Controller/Monster/MonsterBaseController.h"

UBTService_IsLookingInDirection::UBTService_IsLookingInDirection()
{
}

void UBTService_IsLookingInDirection::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	bool beingLookedAt = false;
	
	for (APlayerState* playerState : GetWorld()->GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		APawn* monster = OwnerComp.GetAIOwner()->GetPawn();
		AMainPlayerController* playerController = Cast<AMainPlayerController>(playerState->GetPlayerController());
		if (IsValid(monster) && IsValid(playerController) && playerController->IsLookingAtActor(monster))
		{
			beingLookedAt = true;
			break;
		}
	}
	
	if (beingLookedAt)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
}
