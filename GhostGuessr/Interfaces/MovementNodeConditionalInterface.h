// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovementNodeConditionalInterface.generated.h"

UINTERFACE()
class UMovementNodeConditionalInterface : public UInterface
{
	GENERATED_BODY()
};

class GHOSTGUESSR_API IMovementNodeConditionalInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetPathConditionalSucceeds();
	virtual bool GetPathConditionalSucceeds_Implementation();
};
