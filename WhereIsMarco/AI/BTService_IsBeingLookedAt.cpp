// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsBeingLookedAt.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "UE5Coro/AsyncAwaiters.h"
#include "UE5Coro/TaskAwaiters.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/WhereIsMarcoPlayerState.h"
#include "WhereIsMarco/Controller/Monster/MonsterBaseController.h"
#include "WhereIsMarco/Controller/Player/MainPlayerController.h"

UBTService_IsBeingLookedAt::UBTService_IsBeingLookedAt()
{
	NodeName = "Is AI being looked at by player";
}

void UBTService_IsBeingLookedAt::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* lookingPawn = nullptr;
	bool beingLookedAt = false;
	
	for (APlayerState* playerState : GetWorld()->GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		APawn* monster = OwnerComp.GetAIOwner()->GetPawn();
		AMainPlayerController* playerController = Cast<AMainPlayerController>(playerState->GetPlayerController());
		if (IsValid(monster) && IsValid(playerController) && playerController->CanSeeActor(monster) && playerController->IsLookingAtActor(monster))
		{
			lookingPawn = playerState->GetPawn();
			beingLookedAt = true;
			break;
		}
	}
	
	if (beingLookedAt && IsValid(lookingPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), lookingPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
	
}

// Unused for now
UE5Coro::TCoroutine<> UBTService_IsBeingLookedAt::IsBeingLookedAt(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	check(IsInGameThread());
	
	TWeakObjectPtr<UBTService_IsBeingLookedAt> SelfWeak = this;
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	co_await UE5Coro::Tasks::MoveToTask();
	
	APawn* lookingPawn = nullptr;
	bool beingLookedAt = false;
	
	for (APlayerState* playerState : GetWorld()->GetGameState<AWhereIsMarcoGameState>()->PlayerArray)
	{
		APawn* monster = OwnerComp.GetAIOwner()->GetPawn();
		AMainPlayerController* playerController = Cast<AMainPlayerController>(playerState->GetPlayerController());
		if (IsValid(monster) && IsValid(playerController) && playerController->CanSeeActor(monster) && playerController->IsLookingAtActor(monster))
		{
			lookingPawn = playerState->GetPawn();
			beingLookedAt = true;
			break;
		}
	}
	
	co_await UE5Coro::Async::MoveToGameThread();
	
	if (beingLookedAt && IsValid(lookingPawn))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), lookingPawn);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
	
	co_return;
}

