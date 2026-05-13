// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementNode.h"

#include "GhostGuessr/Interfaces/MovementNodePathInterface.h"

AMovementNode::AMovementNode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovementNode::OnConstruction(const FTransform& Transform)
{
	TArray<TObjectPtr<AActor>> pathsToRemove;
	for (auto path : _MovementPaths)
	{
		if (!IsValid(path))
		{
			pathsToRemove.Add(path);
			continue;
		}
		
		if (path->Implements<UMovementNodePathInterface>())
		{
			if (IMovementNodePathInterface::Execute_GetIsNodeConnectedToPath(path, this))
			{
				path->OnConstruction(Transform);
			}
			else
			{
				pathsToRemove.Add(path);
			}
		}
	}

	for (auto path : pathsToRemove)
	{
		_MovementPaths.Remove(path);
	}

	Super::OnConstruction(Transform);
}

void AMovementNode::BeginPlay()
{
	Super::BeginPlay();
}

void AMovementNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

