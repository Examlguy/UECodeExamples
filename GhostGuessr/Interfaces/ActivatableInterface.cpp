// Fill out your copyright notice in the Description page of Project Settings.


#include "ActivatableInterface.h"


// Add default functionality here for any IActivatableInterface functions that are not pure virtual.
void IActivatableInterface::ToggleActivate_Implementation(bool ActiveState, AController* Activator)
{
}

bool IActivatableInterface::GetIsActivated_Implementation()
{
	return false;
}

FText IActivatableInterface::GetPromptText_Implementation()
{
	return FText();
}
