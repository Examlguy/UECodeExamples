// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

USTRUCT()
struct FItemData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool IsHolding = false;
	UPROPERTY()
	TObjectPtr<APawn> GrabbingPawn = nullptr;
	UPROPERTY()
	TObjectPtr<USceneComponent> GrabbingPawnComponent = nullptr;
	UPROPERTY()
	FName HoldingBoneName = "";
	UPROPERTY()
	float Time = 0;
};

UCLASS()
class COVERTCOUNTEROPS_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPickedUp(APawn* GrabbingPawn, USceneComponent* GrabbingPawnComponent);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDropped();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnStartUsing();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStopUsing();

	UFUNCTION(BlueprintCallable)
	virtual void SetIsUsing(bool NewIsUsing);
	
	UFUNCTION(BlueprintCallable)
	virtual void Pickup(APawn* GrabbingPawn, USceneComponent* GrabbingPawnComponent, FName BoneName = "");

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_ToggleUsing(bool IsUsing);
	
	UFUNCTION(BlueprintCallable)
	virtual void Drop();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool GetIsUsing() const { return _IsUsing; }

	UFUNCTION()
	void OnRep_ItemData();

	UFUNCTION()
	void OnRep_IsUsing(bool LastIsUsing);
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_ItemData)
	FItemData _LatestItemData;
	FItemData _LocalItemData;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsUsing)
	bool _IsUsing = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimSequenceBase> _HoldAnimation;
};
