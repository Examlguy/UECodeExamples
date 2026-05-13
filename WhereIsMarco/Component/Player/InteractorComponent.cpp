// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WhereIsMarco/World/Interactable/Activatable.h"
#include "WhereIsMarco/World/Interactable/Interactable.h"

// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(_interactFromComponent))
	{
		FHitResult hitResult;
		FCollisionQueryParams collisionParams;
		collisionParams.AddIgnoredActor(GetOwner());

		FVector endLocation = _interactFromComponent->GetComponentLocation() + _interactFromComponent->GetForwardVector() * 2000;
		
		GetWorld()->LineTraceSingleByChannel(hitResult, _interactFromComponent->GetComponentLocation(), endLocation, ECC_Visibility, collisionParams);
		
		bool hasInteractable = false;
		if (hitResult.bBlockingHit)
		{
			_currentAimLocation = hitResult.Location;
			
			if (hitResult.Distance <= _interactDistance)
			{
				TArray<TObjectPtr<AActor>> overlapActors;
				TArray<TEnumAsByte<EObjectTypeQuery>> overlapObjectTypes;
				overlapObjectTypes.Add(TEnumAsByte(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel2)));
				TArray<TObjectPtr<AActor>> actorsToIgnore;

				UKismetSystemLibrary::SphereOverlapActors(GetWorld(), hitResult.Location, _nearbyInteractDistance, overlapObjectTypes, AInteractable::StaticClass(), actorsToIgnore,overlapActors);

				if (overlapActors.Num() > 0)
				{
					TObjectPtr<AActor> closestActor = nullptr;
					double lastDistance = -1;
				
					for ( TObjectPtr<AActor> overlappedActor : overlapActors )
					{
						double distanceToOverlapped = UKismetMathLibrary::Vector_Distance(hitResult.Location, overlappedActor->GetActorLocation());
					
						if (lastDistance == -1 || distanceToOverlapped < lastDistance)
						{
							closestActor = overlappedActor;
							lastDistance = distanceToOverlapped;
						}
					}

					if (hitResult.GetActor() == closestActor)
					{
						if (_currentInteractable != closestActor)
						{
							_currentInteractable = Cast<AInteractable>(closestActor);
						}
						
						hasInteractable = true;
					}

					if (hasInteractable == false)
					{
						FHitResult confirmHitResult;
						GetWorld()->LineTraceSingleByChannel(confirmHitResult, _interactFromComponent->GetComponentLocation(), closestActor->GetActorLocation(), ECC_Visibility, collisionParams);

						if (confirmHitResult.GetActor() == closestActor)
						{
							if (_currentInteractable != closestActor)
							{
								_currentInteractable = Cast<AInteractable>(closestActor);
							}
						
							hasInteractable = true;
						}
					}
				}
			}
		}

		if (!hasInteractable)
		{
			_currentInteractable = nullptr;
		}
	}
}

void UInteractorComponent::TriggerCurrentInteractable()
{
	if (_currentInteractable && _currentInteractable->GetIsActivatable())
	{
		bool currentActivatable = !_currentInteractable->GetActivated();
		if (GetOwnerRole() != ROLE_Authority)
		{
			_currentInteractable->PlayerActivate(GetWorld()->GetFirstPlayerController(), currentActivatable);
		}
		
		Server_TriggerCurrentInteractable(GetWorld()->GetFirstPlayerController(), _currentInteractable, currentActivatable);
	}
}

void UInteractorComponent::Server_TriggerCurrentInteractable_Implementation(APlayerController* requesting_controller, AInteractable* interactable, bool requested_active)
{
	if (interactable && interactable->GetIsActivatable())
	{
		interactable->PlayerActivate(requesting_controller, requested_active);
	}
}

