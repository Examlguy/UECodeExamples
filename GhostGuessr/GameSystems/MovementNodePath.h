// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementNode.h"
#include "GameFramework/Actor.h"
#include "GhostGuessr/Interfaces/MovementNodePathInterface.h"
#include "MovementNodePath.generated.h"

UCLASS()
class GHOSTGUESSR_API AMovementNodePath : public AActor, public IMovementNodePathInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMovementNodePath();

	virtual bool GetIsNodeConnectedToPath_Implementation(AMovementNode* MovementNode) override;
	virtual AMovementNode* GetNextNode_Implementation(AMovementNode* FromNode) override;
	virtual bool GetCanUsePath_Implementation() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AMovementNode> _NodeA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AMovementNode> _NodeB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> _PathConditionals;
};
