// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedSightTarget.h"
#include "AdvancedSightTargetComponent.h"
#include "AdvancedSightTeamComponent.h"
#include "GMCPawn.h"
#include "Components/AIUtilityComponent.h"
#include "CovertCounterOps/Items/Item.h"
#include "Interfaces/AIUtility.h"
#include "Interfaces/CommonUtility.h"
#include "Interfaces/Possessable.h"
#include "Camera/CameraComponent.h"
#include "CovertCounterOps/Game/Interfaces/Visibility.h"
#include "Logging/LogMacros.h"

#include "SoldierCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSoldierCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulateTakenDamageEvent, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableLookedAtChanged, AActivatable*, Interactable);

UCLASS(config=Game)
class ASoldierCharacter : public AGMC_Pawn, public IPossessable, public IAIUtility, public IAdvancedSightTarget, public ICommonUtility, public IVisibility
{
	GENERATED_BODY()
public:
	ASoldierCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual TSubclassOf<UUserWidget> GetPossessableHUD_Implementation() override { return _SoldierHUD; }

	virtual void SetTeam_Implementation(int32 TeamNumber) override;
	
	virtual int32 GetTeam_Implementation() override { return _TeamNumber; }

	virtual float GetInteractDistance_Implementation() override { return _InteractDistance; }

	virtual bool CanBePossessedBy_Implementation(AMainPlayerController* PlayerController) override { return false; }

	virtual AItem* GetHeldItem_Implementation() override { return _CurrentHeldItem.Get(); }

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual EActorState GetActorState_Implementation() override { return _SoldierCharacterState; }
	
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticast_HasTakenDamage(float DamageAmount);
	
	virtual UAIUtilityComponent* GetAIUtilityComponent_Implementation() override { return _SoldierAIUtility; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void CheckForInteractable();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_PickUpItem(AItem* PickedUpItem);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPickedUpItemEvent();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_DropItem(AItem* DropItem);

	UFUNCTION(BlueprintCallable)
	void InteractInteractable();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanInteractInteractable();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_InteractInteractable(AActivatable* Activatable);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestStopHoldTrigger(AActivatable* Activatable);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDropItemEvent();
	
	UFUNCTION(BlueprintCallable)
	float GetHealthPercentage() const { return _CurrentHealth / _MaxHealth; }
	
	UFUNCTION()
	void OnRep_TeamNumberChanged();

	UFUNCTION()
	void OnRep_SoldierCharacterState();

	UFUNCTION()
	void OnRep_CurrentHeldItem(TSoftObjectPtr<AItem> LastItem);

	UFUNCTION(BlueprintImplementableEvent)
	void SoldierCharacterStateChangedEvent();

	UPROPERTY(BlueprintAssignable)
	FOnSimulateTakenDamageEvent OnSimulateTakenDamageEventDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteractableLookedAtChanged OnInteractableLookedAtChangedDelegate;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NotifyControllerChanged() override;

	UPROPERTY(ReplicatedUsing=OnRep_TeamNumberChanged)
	int32 _TeamNumber;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_SoldierCharacterState)
	EActorState _SoldierCharacterState = EActorState::NORMAL;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> _SoldierHUD;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAIUtilityComponent> _SoldierAIUtility;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAdvancedSightTeamComponent> _TeamComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAdvancedSightTargetComponent> _SightTargetComponent;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> _SkeletalMeshComponent;

	UPROPERTY()
	TObjectPtr<UCameraComponent> _CameraComponent;
	
	UPROPERTY(EditAnywhere)
	FName _ItemHoldingBoneName;
	
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHeldItem, BlueprintReadWrite)
	TSoftObjectPtr<AItem> _CurrentHeldItem;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TSoftObjectPtr<AActivatable> _CurrentInteractable;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float _CurrentHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float _InteractDistance = 500.f;

private:
	UPROPERTY(EditAnywhere)
	float _MaxHealth = 100;
};

