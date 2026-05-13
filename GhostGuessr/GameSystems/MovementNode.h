// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovementNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeEntered, AActor*, ActorEntered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNodeLeft, AActor*, ActorLeft);

UCLASS()
class GHOSTGUESSR_API AMovementNode : public AActor
{
	GENERATED_BODY()

public:
	AMovementNode();
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<AActor>> _MovementPaths;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnNodeEntered OnNodeEnteredDelegate;
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnNodeEntered FOnNodeLeftDelegate;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementNode", Meta = (MakeEditWidget = true))
	FVector _OffsetFromNode = FVector(0,0,150);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementNode")
	bool _IsNodeOutside = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementNode")
	bool _IsStartNode = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementNode", Meta = (MakeEditWidget = true, EditCondition = "_IsStartNode"))
	FVector _StartLookPoint = FVector(0,0,0);
};
