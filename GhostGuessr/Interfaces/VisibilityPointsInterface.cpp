// Fill out your copyright notice in the Description page of Project Settings.


#include "VisibilityPointsInterface.h"


// Add default functionality here for any IVisibilityPointsInterface functions that are not pure virtual.
TArray<USceneComponent*> IVisibilityPointsInterface::GetVisibilityPoints_Implementation()
{
	return TArray<TObjectPtr<USceneComponent>>();
}
