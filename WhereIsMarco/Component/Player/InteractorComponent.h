// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WhereIsMarco/World/Painting.h"
#include "WhereIsMarco/World/Interactable/Interactable.h"
#include "InteractorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHEREISMARCO_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void TriggerCurrentInteractable();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_TriggerCurrentInteractable(APlayerController* requesting_controller, AInteractable* interactable, bool requested_active);
	
	UFUNCTION(BlueprintCallable)
	void SetInteractFromComponent(USceneComponent* interactor_component) { _interactFromComponent = interactor_component; }

	UFUNCTION(BlueprintCallable)
	float GetInteractDistance() { return _interactDistance; }
	
	UFUNCTION(BlueprintCallable)
	AInteractable* GetCurrentInteractable() { return _currentInteractable; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactor")
	float _interactDistance = 250.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactor")
	float _nearbyInteractDistance = 50.0f;

	UPROPERTY(BlueprintReadOnly)
	FVector _currentAimLocation;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintSetter=SetInteractFromComponent)
	TObjectPtr<USceneComponent> _interactFromComponent;
	
	UPROPERTY()
	TObjectPtr<AInteractable> _currentInteractable;
};
