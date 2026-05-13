// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GMCAbilityComponent.h"
#include "PossessableCharacter.h"
#include "UPlayerGoldSourceMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "WhereIsMarco/Character/Monster/MonsterBaseCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class WHEREISMARCO_API APlayerCharacter : public APossessableCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerCharacter();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void SetMovementEnabled(bool enabled) override;

	virtual void LookAction(const FInputActionValue& input_value) override;
	virtual void MoveAction(const FInputActionValue& input_value) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowMarcoLocationScreenshot(int key);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> _skeletalMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> _cameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> _capsuleComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UPlayerGoldSourceMovementComponent> _playerMovementComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UGMC_AbilitySystemComponent> _abilitySystemComponent;
};
