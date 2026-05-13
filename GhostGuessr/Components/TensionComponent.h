// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostGuessr/Interfaces/TensionInterface.h"
#include "TensionComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GHOSTGUESSR_API UTensionComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UTensionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyTension(ETensionType TensionType, float Multiplier);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float _MaxTension = 100;

	UPROPERTY(BlueprintReadWrite)
	float _CurrentTension = 0;
};
