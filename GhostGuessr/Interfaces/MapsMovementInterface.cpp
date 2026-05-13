// Fill out your copyright notice in the Description page of Project Settings.


#include "MapsMovementInterface.h"


// Add default functionality here for any IMapsMovementInterface functions that are not pure virtual.
bool IMapsMovementInterface::GetIsOutside_Implementation()
{
	return true;
}

UTensionComponent* IMapsMovementInterface::GetTensionComponent_Implementation()
{
	return nullptr;
}
