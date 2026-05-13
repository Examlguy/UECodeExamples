// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WhereIsMarco/Character/WhereIsMarcoBaseCharacter.h"
#include "MonsterBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	IDLE = 0,
	SEARCHING = 1,
	STALKING = 2,
	CHASING = 3,
	FLEEING = 4
};

UCLASS()
class WHEREISMARCO_API AMonsterBaseCharacter : public AWhereIsMarcoBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMonsterBaseCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState(EMovementState movement_state);

	UFUNCTION(BlueprintImplementableEvent)
	void MovementStateChangedEvent(EMovementState movement_state);

	UFUNCTION(BlueprintCallable)
	EMovementState GetCurrentMovementState() const { return _currentMovementState; }
	
	UFUNCTION(BlueprintCallable)
	USceneComponent* GetScareLookAtLocation() const { return _scareLookAtLocation; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> _capsuleComponent;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> _scareLookAtLocation;

	UPROPERTY()
	EMovementState _currentMovementState = EMovementState::IDLE;
};
