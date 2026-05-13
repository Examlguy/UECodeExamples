// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlaceAreaCheckMarker.h"

#include "WhereIsMarco/Controller/Monster/MonsterBaseController.h"

UBTTask_PlaceAreaCheckMarker::UBTTask_PlaceAreaCheckMarker()
{
	NodeName = "Place Area Checked Marker";
}

EBTNodeResult::Type UBTTask_PlaceAreaCheckMarker::ExecuteTask(class UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	
	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterBaseController* monsterController = Cast<AMonsterBaseController>(OwnerComp.GetAIOwner());

	if (monsterController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	monsterController->PlaceCheckedAreaMarker(monsterController->GetPawn()->GetActorLocation());

	return EBTNodeResult::Succeeded;
}
