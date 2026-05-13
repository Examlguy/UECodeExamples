// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EffectsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UEffectsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTGUESSR_API IEffectsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyCameraShake(float StartDelay, float Duration);
	virtual void ApplyCameraShake_Implementation(float StartDelay, float Duration);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyCameraGlitch(float StartDelay, float Duration);
	virtual void ApplyCameraGlitch_Implementation(float StartDelay, float Duration);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyZoom(float StartDelay, float Duration);
	virtual void ApplyZoom_Implementation(float StartDelay, float Duration);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyScreenDisable(float StartDelay, float Duration);
	virtual void ApplyScreenDisable_Implementation(float StartDelay, float Duration);
};
