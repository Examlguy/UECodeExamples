// Fill out your copyright notice in the Description page of Project Settings.


#include "PossessableCharacter.h"
#include "InputActionValue.h"

APossessableCharacter::APossessableCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APossessableCharacter::LookAction(const FInputActionValue& input_value)
{
	if (_movementEnabled)
	{
		LookActionEvent(input_value);
	}
}

void APossessableCharacter::MoveAction(const FInputActionValue& input_value)
{
	if (_movementEnabled)
	{
		MoveActionEvent(input_value);
	}
}

void APossessableCharacter::PrimaryAction()
{
	PrimaryActionEvent();
}

void APossessableCharacter::SecondaryAction()
{
	SecondaryActionEvent();
}

void APossessableCharacter::SprintAction()
{
	if (_movementEnabled)
	{
		SprintActionEvent();
	}
}

void APossessableCharacter::CrouchAction()
{
	if (_movementEnabled)
	{
		CrouchActionEvent();
	}
}

void APossessableCharacter::InteractionAction()
{
	InteractionActionEvent();
}

void APossessableCharacter::EscapeAction()
{
	EscapeActionEvent();
}

void APossessableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APossessableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APossessableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

