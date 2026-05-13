// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "GhostGuessr/GameSystems/MovementNode.h"
#include "GhostGuessr/Interfaces/ControlInputsInterface.h"
#include "GhostGuessr/Interfaces/MapsMovementInterface.h"
#include "MapsMovementPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelState, bool, IsTravelling);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeftNode, AMovementNode*, LeftNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnteredNode, AMovementNode*, EnteredNode);

UCLASS()
class GHOSTGUESSR_API AMapsMovementPawn : public ASpectatorPawn, public IControlInputsInterface, public IMapsMovementInterface
{
	GENERATED_BODY()

public:
	AMapsMovementPawn();

	virtual void BeginPlay() override;
	virtual void AddControllerPitchInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CalculateAngularVelocity(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintCallable, BlueprintAssignable, Category = "MapsMovementPawn")
	FOnTravelState OnTravelStateDelegate;
	UPROPERTY(EditAnywhere, BlueprintCallable, BlueprintAssignable, Category = "MapsMovementPawn")
	FOnLeftNode OnLeftNodeDelegate;
	UPROPERTY(EditAnywhere, BlueprintCallable, BlueprintAssignable, Category = "MapsMovementPawn")
	FOnEnteredNode OnEnteredNodeDelegate;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "MapsMovementPawn")
	float _LastYawInput;
	UPROPERTY(BlueprintReadOnly, Category = "MapsMovementPawn")
	float _LastPitchInput;
	UPROPERTY(BlueprintReadWrite, Category = "MapsMovementPawn")
	float _SensitivityMultiplier = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category = "MapsMovementPawn")
	FVector _AngularVelocity;
	UPROPERTY(BlueprintReadOnly, Category = "MapsMovementPawn")
	FRotator _LastRotation;
};
