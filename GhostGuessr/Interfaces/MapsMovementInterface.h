// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostGuessr/Components/TensionComponent.h"
#include "UObject/Interface.h"
#include "MapsMovementInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMapsMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class GHOSTGUESSR_API IMapsMovementInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsOutside();
	virtual bool GetIsOutside_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UTensionComponent* GetTensionComponent();
	virtual UTensionComponent* GetTensionComponent_Implementation();
};
