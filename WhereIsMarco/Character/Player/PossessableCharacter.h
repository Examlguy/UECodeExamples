// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WhereIsMarco/Character/WhereIsMarcoBaseCharacter.h"
#include "PossessableCharacter.generated.h"

struct FInputActionValue;

UCLASS()
class WHEREISMARCO_API APossessableCharacter : public AWhereIsMarcoBaseCharacter
{
	GENERATED_BODY()
public:
	APossessableCharacter();

	virtual void LookAction(const FInputActionValue& input_value);
	virtual void MoveAction(const FInputActionValue& input_value);
	virtual void PrimaryAction();
	virtual void SecondaryAction();
	virtual void SprintAction();
	virtual void CrouchAction();
	virtual void EscapeAction();
	virtual void InteractionAction();

	UFUNCTION(BlueprintImplementableEvent)
	void LookActionEvent(const FInputActionValue& input_value);
	UFUNCTION(BlueprintImplementableEvent)
	void MoveActionEvent(const FInputActionValue& input_value);
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryActionEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void SecondaryActionEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void SprintActionEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void CrouchActionEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void EscapeActionEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void InteractionActionEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
