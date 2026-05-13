// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlInputsInterface.h"

#include "GameFramework/InputSettings.h"


// Add default functionality here for any IControlInputsInterface functions that are not pure virtual.
void IControlInputsInterface::ReserveCantMove_Implementation(UObject* Instigator)
{
}

void IControlInputsInterface::UnreserveCantMove_Implementation(UObject* Instigator)
{
}

bool IControlInputsInterface::GetCanMove_Implementation()
{
	return true;
}

void IControlInputsInterface::ReserveCantLook_Implementation(UObject* Instigator)
{
}

void IControlInputsInterface::UnreserveCantLook_Implementation(UObject* Instigator)
{
}

bool IControlInputsInterface::GetCanLook_Implementation()
{
	return true;
}

void IControlInputsInterface::SetInputDevice_Implementation(EHardwareDevicePrimaryType InputDevice)
{
	// no op
}

EHardwareDevicePrimaryType IControlInputsInterface::GetInputDevice_Implementation()
{
	return EHardwareDevicePrimaryType::KeyboardAndMouse;
}
