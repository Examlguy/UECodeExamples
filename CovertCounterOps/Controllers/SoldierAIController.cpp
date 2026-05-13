#include "SoldierAIController.h"

#include "CovertCounterOps/Characters/Interfaces/AIUtility.h"
#include "AdvancedSightTargetComponent.h"
#include "AdvancedSightVisibilityPointComponent.h"
#include "CovertCounterOps/Characters/Interfaces/CommonUtility.h"

ASoldierAIController::ASoldierAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	_AdvancedSightComponent = CreateDefaultSubobject<UAdvancedSightComponent>(TEXT("AdvancedSightComponent"));
	_HTNComponent = CreateDefaultSubobject<UHTNComponent>(TEXT("HTNComponent"));
}

void ASoldierAIController::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(_BehaviourBrain))
	{
		_HTNComponent->StartHTN(_BehaviourBrain);
	}
}

void ASoldierAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ASoldierAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	FVector eyePOV = _AdvancedSightComponent->GetEyePointOfViewTransform().GetLocation();
	if (IsValid(Actor) && Actor->Implements<UCommonUtility>())
	{
		UAdvancedSightTargetComponent* targetComponent = ICommonUtility::Execute_GetPointVisibilityComponent(const_cast<AActor*>(Actor));
		if (IsValid(targetComponent))
		{
			for (auto viewpoint : targetComponent->GetVisibilityPointComponents())
			{
				FHitResult Hit;
				GetWorld()->LineTraceSingleByChannel(Hit, eyePOV, viewpoint->GetComponentLocation(), ECollisionChannel::ECC_Visibility);
				if (Hit.GetActor() == Actor)
				{
					return viewpoint->GetComponentLocation();
				}
			}
		}
	}
	
	return Super::GetFocalPointOnActor(Actor);
}

bool ASoldierAIController::LineOfSightTo(const AActor* Other, FVector ViewPoint, bool bAlternateChecks) const
{
	FVector eyePOV = _AdvancedSightComponent->GetEyePointOfViewTransform().GetLocation();
	if (IsValid(Other) && Other->Implements<UCommonUtility>())
	{
		UAdvancedSightTargetComponent* targetComponent = ICommonUtility::Execute_GetPointVisibilityComponent(const_cast<AActor*>(Other));
		if (IsValid(targetComponent))
		{
			for (auto targetViewpoint : targetComponent->GetVisibilityPointComponents())
			{
				FHitResult Hit;
				GetWorld()->LineTraceSingleByChannel(Hit, eyePOV, targetViewpoint->GetComponentLocation(), ECollisionChannel::ECC_Visibility);
				if (Hit.GetActor() == Other)
				{
					return true;
				}
			}
		}
	}
	
	return Super::LineOfSightTo(Other, eyePOV, bAlternateChecks);
}

void ASoldierAIController::UpdateControlRotation(float DeltaTime, bool ShouldUpdatePawn)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		SetControlRotation(NewControlRotation);

		if (ShouldUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}

float ASoldierAIController::DistanceToTarget(AActor* InTarget)
{
	return FVector::Dist(GetPawn()->GetActorLocation(), InTarget->GetActorLocation());
}

void ASoldierAIController::ProcessVisionedTarget(AActor* InTarget, ESpotState SpotState)
{
	APawn* targetPawn = Cast<APawn>(InTarget);
	if (!IsValid(targetPawn))
	{
		return;
	}

	APawn* controllerPawn = GetPawn();
	if (!IsValid(controllerPawn))
	{
		return;
	}

	UAIUtilityComponent* utilityCmp = IAIUtility::Execute_GetAIUtilityComponent(controllerPawn);
	if (!IsValid(utilityCmp))
	{
		return;
	}
	
	APawn* currentTarget = utilityCmp->GetCurrentTarget();
	if (currentTarget == targetPawn)
	{
		OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), targetPawn, SpotState);
	}
	else
	{
		if (!IsValid(currentTarget))
		{
			OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), targetPawn, SpotState);
			return;
		}
		
		if (SpotState == ESpotState::PERCEIVED)
		{
			if (DistanceToTarget(targetPawn) < DistanceToTarget(currentTarget))
			{
				OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), targetPawn, SpotState);
				return;
			}
		}

		if (SpotState == ESpotState::PERCEIVED || SpotState == ESpotState::SPOTTED)
		{
			if (LineOfSightTo(targetPawn) && !LineOfSightTo(currentTarget))
			{
				OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), targetPawn, SpotState);
			}
		}
	}
}

void ASoldierAIController::GetNextPriorityTarget()
{
	{
		TArray<AActor*> perceivedTargets = _AdvancedSightComponent->GetPerceivedTargets();
		if (perceivedTargets.Num() > 0)
		{
			AActor* closestTarget = perceivedTargets[0];
			for (auto target : perceivedTargets)
			{
				if (DistanceToTarget(target) < DistanceToTarget(closestTarget))
				{
					closestTarget = target;
				}
			}

			if (APawn* newTarget = Cast<APawn>(closestTarget))
			{
				OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), newTarget, ESpotState::PERCEIVED);
			}
			return;
		}
	}

	{
		TArray<AActor*> spottedTargets = _AdvancedSightComponent->GetSpottedTargets();
		if (spottedTargets.Num() > 0)
		{
			AActor* closestTarget = spottedTargets[0];
			for (auto target : spottedTargets)
			{
				if (DistanceToTarget(target) < DistanceToTarget(closestTarget))
				{
					closestTarget = target;
				}
			}

			if (APawn* newTarget = Cast<APawn>(closestTarget))
			{
				OnTargetPerceptionStateDelegate.Broadcast(GetPawn(), newTarget, ESpotState::SPOTTED);
			}
		}
	}
}
