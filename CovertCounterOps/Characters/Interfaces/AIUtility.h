// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CovertCounterOps/Characters/Components/AIUtilityComponent.h"
#include "UObject/Interface.h"
#include "AIUtility.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAIUtility : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COVERTCOUNTEROPS_API IAIUtility
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UAIUtilityComponent* GetAIUtilityComponent();
	virtual UAIUtilityComponent* GetAIUtilityComponent_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitialiseAIPawn();
	virtual void InitialiseAIPawn_Implementation();
};
