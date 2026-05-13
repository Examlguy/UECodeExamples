// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Activatable.generated.h"

USTRUCT(BlueprintType)
struct FActivatableState
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	bool activated = false;
	UPROPERTY()
	TObjectPtr<APawn> instigatingPawn = nullptr;
	UPROPERTY()
	float time = -1;
	UPROPERTY()
	bool correction = false;
};

UCLASS(Blueprintable)
class COVERTCOUNTEROPS_API AActivatable : public AActor
{
	GENERATED_BODY()

public:
	AActivatable();
	
	UFUNCTION(BlueprintCallable)
	float GetPercentageHeldTrigger() const;

	bool CanHoldTrigger() const { return IsValid(_triggerHoldingPawn); }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRepNotify_ActivatedChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHoldActivationEvent(bool Started);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnActivationChangedEvent();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivatableCorrection(FActivatableState correct_activatable_state);

	UFUNCTION(BlueprintCallable)
	void StartHoldTrigger(APawn* instigating_pawn);

	UFUNCTION(BlueprintCallable)
	void StopHoldTrigger(APawn* instigating_pawn);

	UFUNCTION()
	void CheckIfPlayerStillHoldingTrigger();

	UFUNCTION(BlueprintCallable)
	void PawnTriggeredActivate(APawn* instigating_pawn, bool requested_active);

	UFUNCTION(BlueprintCallable)
	void ToggleActivate(bool requested_active);

	UFUNCTION(BlueprintCallable)
	bool GetActivated() const { return _activatableState.activated; }

	UFUNCTION(BlueprintCallable)
	APawn* GetInstigatingActor() const { return _activatableState.instigatingPawn; }

	UFUNCTION(BlueprintCallable)
	bool GetStartActivated() const { return _startActivated; }

	UFUNCTION(BlueprintCallable)
	bool GetIsHoldToActivate() const { return _isHoldToActivate; }
	
	virtual FText GetActivatablePromptText() const
	{
		return _activatableState.activated ?
		_playerDeactivatable ? _deactivateText : _notDeactivatableText :
		_playerActivatable ? _activateText : _notActivatableText;
	};

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void SetActivated(FActivatableState activated_state);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	FText _activateText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	FText _notActivatableText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	FText _deactivateText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	FText _notDeactivatableText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	bool _showPromptText = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	bool _playerActivatable = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	bool _playerDeactivatable = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	bool _startActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activatable")
	bool _isHoldToActivate = false;

	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Activatable")
	float _holdTimeForActivate = 5.f;

	UPROPERTY(BlueprintReadOnly)
	float _startHoldTimeForActivate = 0;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<APawn> _triggerHoldingPawn;
	
	UPROPERTY(ReplicatedUsing=OnRepNotify_ActivatedChanged)
	FActivatableState _activatableState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimSequenceBase> _holdActivateAnimation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimSequenceBase> _activateAnimation;
};
