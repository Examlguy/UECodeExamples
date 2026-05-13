// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonUtility.h"

#include "CovertCounterOps/Game/A1VXGameState.h"


// Add default functionality here for any ICommonUtility functions that are not pure virtual.
void ICommonUtility::SetTeam_Implementation(int32 TeamNumber)
{
	// no op
}

int32 ICommonUtility::GetTeam_Implementation()
{
	return static_cast<int32>(ETeam::UNASSIGNED);
}

AItem* ICommonUtility::GetHeldItem_Implementation()
{
	return nullptr;
}

TArray<AItem*> ICommonUtility::GetInventory_Implementation()
{
	return TArray<AItem*>();
}

float ICommonUtility::GetInteractDistance_Implementation()
{
	return 500.f;
}

URollbackComponent* ICommonUtility::GetRollbackComponent_Implementation()
{
	return nullptr;
}

UCapsuleComponent* ICommonUtility::GetHitBoxCollisionComponent_Implementation()
{
	return nullptr;
}

void ICommonUtility::ToggleMainCollision_Implementation(bool Enabled)
{
	// no op
}

FVector ICommonUtility::GetEyesViewPoint_Implementation()
{
	return FVector::ZeroVector;
}

EActorState ICommonUtility::GetActorState_Implementation()
{
	return EActorState::DEAD;
}

UAdvancedSightTargetComponent* ICommonUtility::GetPointVisibilityComponent_Implementation()
{
	return nullptr;
}
