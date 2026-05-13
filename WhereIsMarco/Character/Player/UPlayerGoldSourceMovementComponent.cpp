// Fill out your copyright notice in the Description page of Project Settings.


#include "UPlayerGoldSourceMovementComponent.h"


// Sets default values for this component's properties
UPlayerGoldSourceMovementComponent::UPlayerGoldSourceMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerGoldSourceMovementComponent::ReceiveMovementInput(const FInputActionValue& input_value)
{
	if (!IsValid(PawnOwner))
	{
		return;
	}
	
	AddInputVector(input_value.Get<FVector>());
}

void UPlayerGoldSourceMovementComponent::ReceiveLookInput(const FInputActionValue& input_value)
{
	if (!IsValid(PawnOwner))
	{
		return;
	}

	const FVector& Value = input_value.Get<FVector>();
	
	PawnOwner->AddControllerRollInput(Value.Z);
	PawnOwner->AddControllerPitchInput(-Value.Y);
	PawnOwner->AddControllerYawInput(Value.X);
}

void UPlayerGoldSourceMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerGoldSourceMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

