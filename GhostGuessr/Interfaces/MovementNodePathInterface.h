// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostGuessr/GameSystems/MovementNode.h"
#include "UObject/Interface.h"
#include "MovementNodePathInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMovementNodePathInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTGUESSR_API IMovementNodePathInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsNodeConnectedToPath(AMovementNode* MovementNode);
	virtual bool GetIsNodeConnectedToPath_Implementation(AMovementNode* MovementNode);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AMovementNode* GetNextNode(AMovementNode* FromNode);
	virtual AMovementNode* GetNextNode_Implementation(AMovementNode* FromNode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetCanUsePath();
	virtual bool GetCanUsePath_Implementation();
	
};
