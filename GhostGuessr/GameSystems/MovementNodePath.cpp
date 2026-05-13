#include "MovementNodePath.h"

#include "GhostGuessr/Interfaces/MovementNodeConditionalInterface.h"
#include "Kismet/KismetMathLibrary.h"

AMovementNodePath::AMovementNodePath()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool AMovementNodePath::GetIsNodeConnectedToPath_Implementation(AMovementNode* MovementNode)
{
	return MovementNode == _NodeA || MovementNode == _NodeB;
}

AMovementNode* AMovementNodePath::GetNextNode_Implementation(AMovementNode* FromNode)
{
	if (FromNode == _NodeA)
	{
		return _NodeB;
	}

	if (FromNode == _NodeB)
	{
		return _NodeA;
	}

	return nullptr;
}

bool AMovementNodePath::GetCanUsePath_Implementation()
{
	for (auto conditional : _PathConditionals)
	{
		if (conditional->Implements<UMovementNodeConditionalInterface>())
		{
			if (!IMovementNodeConditionalInterface::Execute_GetPathConditionalSucceeds(conditional))
			{
				return false;
			}
		}
	}
	
	return true;
}

void AMovementNodePath::OnConstruction(const FTransform& Transform)
{
	if (IsValid(_NodeA))
	{
		_NodeA->_MovementPaths.AddUnique(this);
	}
	if (IsValid(_NodeB))
	{
		_NodeB->_MovementPaths.AddUnique(this);
	}
	if (IsValid(_NodeA) && IsValid(_NodeB))
	{
		FVector nodeDirection = UKismetMathLibrary::GetDirectionUnitVector(_NodeA->GetActorLocation(), _NodeB->GetActorLocation());
		float nodeDistanceMidpoint = UKismetMathLibrary::Vector_Distance(_NodeA->GetActorLocation(), _NodeB->GetActorLocation()) / 2;

		FVector midPoint = nodeDirection * nodeDistanceMidpoint + _NodeA->GetActorLocation();
		SetActorLocation(midPoint);
	}

	Super::OnConstruction(Transform);
}

// Called when the game starts or when spawned
void AMovementNodePath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMovementNodePath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

