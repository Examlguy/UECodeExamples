// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CovertCounterOps/Characters/Components/RollbackComponent.h"
#include "CovertCounterOps/Items/Item.h"
#include "UObject/Interface.h"
#include "CommonUtility.generated.h"

UENUM(Blueprintable)
enum class EActorState : uint8
{
	NORMAL,
	BUSY,
	STUNNED,
	DEAD
};

// This class does not need to be modified.
UINTERFACE()
class UCommonUtility : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COVERTCOUNTEROPS_API ICommonUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTeam(int32 TeamNumber);
	virtual void SetTeam_Implementation(int32 TeamNumber);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetTeam();
	virtual int32 GetTeam_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AItem* GetHeldItem();
	virtual AItem* GetHeldItem_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<AItem*> GetInventory();
	virtual TArray<AItem*> GetInventory_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetInteractDistance();
	virtual float GetInteractDistance_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	URollbackComponent* GetRollbackComponent();
	virtual URollbackComponent* GetRollbackComponent_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UCapsuleComponent* GetHitBoxCollisionComponent();
	virtual UCapsuleComponent* GetHitBoxCollisionComponent_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ToggleMainCollision(bool Enabled);
	virtual void ToggleMainCollision_Implementation(bool Enabled);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetEyesViewPoint();
	virtual FVector GetEyesViewPoint_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EActorState GetActorState();
	virtual EActorState GetActorState_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UAdvancedSightTargetComponent* GetPointVisibilityComponent();
	virtual UAdvancedSightTargetComponent* GetPointVisibilityComponent_Implementation();

};
