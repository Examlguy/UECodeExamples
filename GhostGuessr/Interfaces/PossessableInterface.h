// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "UObject/Interface.h"
#include "PossessableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPossessableInterface : public UInterface
{
	GENERATED_BODY()
};

class GHOSTGUESSR_API IPossessableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UInputMappingContext* GetControlMapping();
	virtual UInputMappingContext* GetControlMapping_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanBePossessedBy(AController* Controller);
	virtual bool CanBePossessedBy_Implementation(AController* Controller);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TSubclassOf<UUserWidget> GetPossessableHUD();
	virtual TSubclassOf<UUserWidget> GetPossessableHUD_Implementation();
};
