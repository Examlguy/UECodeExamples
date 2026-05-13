// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Visibility.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UVisibility : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COVERTCOUNTEROPS_API IVisibility
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetVisibility(bool IsVisible);
	virtual void SetVisibility_Implementation(bool IsVisible);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCloaked(bool IsCloaked);
	virtual void SetCloaked_Implementation(bool IsCloaked);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsCloaked();
	virtual bool IsCloaked_Implementation();
};
