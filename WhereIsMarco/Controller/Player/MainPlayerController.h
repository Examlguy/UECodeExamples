// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/GMCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "GMCAbilityComponent.h"
#include "WhereIsMarco/Character/Player/PossessableCharacter.h"
#include "MainPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientPossessionChanged, APawn*, new_pawn);

UCLASS()
class WHEREISMARCO_API AMainPlayerController : public AGMC_PlayerController
{
	GENERATED_BODY()
public:
	AMainPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> PrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> EscapeAction;

	virtual void PlayerTick(float DeltaTime) override;
	
	virtual void InitPlayerState() override;
	
	UFUNCTION(BlueprintCallable)
	void LookAtSceneComponent(USceneComponent* look_location, float duration = 0.0f);
	
	UFUNCTION(BlueprintCallable)
	void StopLookAtTarget();

	UFUNCTION(BlueprintCallable)
	void ZoomIn();

	UFUNCTION(BlueprintCallable)
	void ZoomOut();

	UFUNCTION(BlueprintCallable)
	void SetPlayerFOV(float new_angle);

	UFUNCTION(BlueprintCallable)
	bool IsLookingAtActor(AActor* looking_actor);
	
	UFUNCTION(BlueprintCallable)
	bool CanSeeActor(AActor* target_actor);

	UPROPERTY(BlueprintAssignable)
	FOnClientPossessionChanged OnClientPossessionChangedDelegate;
	
protected:
	void ProcessLookAtTarget(float delta_time);
	void ProcessZoom(float delta_time);
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* in_pawn) override;
	// Called on client after possession is done
	virtual void AcknowledgePossession(APawn* in_pawn) override;
	
	void MoveInput(const FInputActionValue& input_value);
	void LookInput(const FInputActionValue& input_value);
	void PrimaryActionTriggered();
	void SecondaryActionTriggered();
	void CrouchActionTriggered();
	void SprintActionTriggered();
	void InteractActionTriggered();
	void EscapeActionTriggered();

private:
	UPROPERTY()
	bool _shouldProcessLookAtTarget = false;
	TObjectPtr<USceneComponent> _lookAtTarget;

	UPROPERTY()
	bool _shouldZoomIn = false;
	
	UPROPERTY(BlueprintSetter=SetPlayerFOV)
	float _defaultFOVAngle;
	
	UPROPERTY()
	TObjectPtr<APossessableCharacter> _currentPossessedCharacter;

	UPROPERTY()
	FTimerHandle _transitionTimer;
};
