// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TensionInterface.generated.h"

UENUM(BlueprintType)
enum class ETensionType : uint8
{
	MOVING,
	SOUND,
	POPUP,
	MONSTER,
	DEATH
};

UINTERFACE()
class UTensionInterface : public UInterface
{
	GENERATED_BODY()
};

class GHOSTGUESSR_API ITensionInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyTension(ETensionType TensionType, float Multiplier);
	virtual void ApplyTension_Implementation(ETensionType TensionType, float Multiplier);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetTension();
	virtual float GetTension_Implementation();
};
