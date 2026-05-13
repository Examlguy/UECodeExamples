// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AreaSpawnNode.generated.h"

UCLASS()
class GHOSTGUESSR_API AAreaSpawnNode : public AActor
{
	GENERATED_BODY()

public:
	AAreaSpawnNode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FVector GetRandomLocationInNavigatableArea();

protected:
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UBoxComponent> _BoxComponent;

};
