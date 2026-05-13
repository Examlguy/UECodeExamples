// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ControlInputsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UControlInputsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GHOSTGUESSR_API IControlInputsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReserveCantMove(UObject* Instigator);
	virtual void ReserveCantMove_Implementation(UObject* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnreserveCantMove(UObject* Instigator);
	virtual void UnreserveCantMove_Implementation(UObject* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetCanMove();
	virtual bool GetCanMove_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ReserveCantLook(UObject* Instigator);
	virtual void ReserveCantLook_Implementation(UObject* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnreserveCantLook(UObject* Instigator);
	virtual void UnreserveCantLook_Implementation(UObject* Instigator);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetCanLook();
	virtual bool GetCanLook_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetInputDevice(EHardwareDevicePrimaryType InputDevice);
	virtual void SetInputDevice_Implementation(EHardwareDevicePrimaryType InputDevice);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EHardwareDevicePrimaryType GetInputDevice();
	virtual EHardwareDevicePrimaryType GetInputDevice_Implementation();
	
};
