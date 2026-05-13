// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GMCGoldSrcMovementComponent.h"
#include "UPlayerGoldSourceMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHEREISMARCO_API UPlayerGoldSourceMovementComponent : public UGMC_GoldSrcMovementCmp
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerGoldSourceMovementComponent();

	UFUNCTION(BlueprintCallable)
	void ReceiveMovementInput(const FInputActionValue& input_value);
	
	UFUNCTION(BlueprintCallable)
	void ReceiveLookInput(const FInputActionValue& input_value);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
