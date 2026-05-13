// Fill out your copyright notice in the Description page of Project Settings.


#include "MapsMovementPawn.h"

#include "GameFramework/InputSettings.h"


// Sets default values
AMapsMovementPawn::AMapsMovementPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMapsMovementPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AMapsMovementPawn::AddControllerPitchInput(float Val)
{
	if (IControlInputsInterface::Execute_GetCanLook(this))
	{
		float modifiedVal = (IControlInputsInterface::Execute_GetInputDevice(this) == EHardwareDevicePrimaryType::KeyboardAndMouse ? -Val : Val) * _SensitivityMultiplier;
		Super::AddControllerPitchInput(modifiedVal);
		_LastPitchInput = modifiedVal;
	}
}

void AMapsMovementPawn::AddControllerYawInput(float Val)
{
	if (IControlInputsInterface::Execute_GetCanLook(this))
	{
		float modifiedVal = (IControlInputsInterface::Execute_GetInputDevice(this) == EHardwareDevicePrimaryType::KeyboardAndMouse ? -Val : Val) * _SensitivityMultiplier;
		Super::AddControllerYawInput(modifiedVal);
		_LastYawInput = modifiedVal;
	}
}

// Called every frame
void AMapsMovementPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateAngularVelocity(DeltaTime);
}

// Called to bind functionality to input
void AMapsMovementPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMapsMovementPawn::CalculateAngularVelocity(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	FRotator DeltaRot = (CurrentRotation - _LastRotation).GetNormalized();

	_AngularVelocity = FVector(DeltaRot.Pitch, DeltaRot.Yaw, DeltaRot.Roll) / DeltaTime;
	_LastRotation = CurrentRotation;
}

