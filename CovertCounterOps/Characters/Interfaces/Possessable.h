// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "CovertCounterOps/Items/Item.h"
#include "UObject/Interface.h"
#include "Possessable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPossessable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COVERTCOUNTEROPS_API IPossessable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UInputMappingContext* GetControlMapping();
	virtual UInputMappingContext* GetControlMapping_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanBePossessedBy(AMainPlayerController* PlayerController);
	virtual bool CanBePossessedBy_Implementation(AMainPlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TSubclassOf<UUserWidget> GetPossessableHUD();
	virtual TSubclassOf<UUserWidget> GetPossessableHUD_Implementation();
};
