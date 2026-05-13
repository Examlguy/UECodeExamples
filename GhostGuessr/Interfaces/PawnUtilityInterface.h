// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUtilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPawnUtilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTGUESSR_API IPawnUtilityInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void KillPawn();
	virtual void KillPawn_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool GetIsPawnDead();
    virtual bool GetIsPawnDead_Implementation();
};
