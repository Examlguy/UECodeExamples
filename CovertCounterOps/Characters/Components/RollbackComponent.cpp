// Fill out your copyright notice in the Description page of Project Settings.


#include "RollbackComponent.h"

#include "CovertCounterOps/Characters/Interfaces/CommonUtility.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
URollbackComponent::URollbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void URollbackComponent::TickComponent(float DeltaTime, ELevelTick TickType,
									   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ProcessFrameHistory();
}

FServerSideRewindResult URollbackComponent::ServerSideRewind(APawn* HitCharacter, const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation, float HitTime)
{
	if (!IsValid(HitCharacter) || !HitCharacter->Implements<UCommonUtility>())
	{
		return FServerSideRewindResult();
	}
	
	URollbackComponent* rollBackComponent = ICommonUtility::Execute_GetRollbackComponent(HitCharacter);
	if (!IsValid(HitCharacter) ||
		rollBackComponent == nullptr ||
		rollBackComponent->_FrameHistory.GetHead() == nullptr ||
		rollBackComponent->_FrameHistory.GetTail() == nullptr)
	{
		return FServerSideRewindResult{false};
	}
	
	const TDoubleLinkedList<FRollbackFrame>& history = rollBackComponent->_FrameHistory;
	const float oldestHistoryTime = history.GetTail()->GetValue().time;
	const float newestHistoryTime = history.GetHead()->GetValue().time;

	if (oldestHistoryTime > HitTime)
	{
		return FServerSideRewindResult{false}; // too far back to rewind
	}
	
	bool shouldInterpolate = true;
	FRollbackFrame frameToCheck;
	
	if (oldestHistoryTime == HitTime)
	{
		frameToCheck = history.GetTail()->GetValue();
		shouldInterpolate = false;
	}
	if (newestHistoryTime <= HitTime)
	{
		frameToCheck = history.GetHead()->GetValue();
		shouldInterpolate = false;
	}

	TDoubleLinkedList<FRollbackFrame>::TDoubleLinkedListNode* younger = history.GetHead();
	TDoubleLinkedList<FRollbackFrame>::TDoubleLinkedListNode* older = younger;
	while (older->GetValue().time > HitTime)
	{
		if (older->GetNextNode() == nullptr)
		{
			break;
		}
		older = older->GetNextNode();
		if (older->GetValue().time > HitTime)
		{
			younger = older;
		}
	}
	if (older->GetValue().time == HitTime)
	{
		frameToCheck = older->GetValue();
		shouldInterpolate = false;
	}
	
	if (shouldInterpolate)
	{
		frameToCheck = InterpBetweenFrames(older->GetValue(), younger->GetValue(), HitTime);
	}

	return ConfirmHit(frameToCheck, HitCharacter, TraceStart, HitLocation);
}

FRollbackFrame URollbackComponent::InterpBetweenFrames(const FRollbackFrame& OlderFrame, const FRollbackFrame& NewerFrame, float HitTime)
{
	const float distance = NewerFrame.time - OlderFrame.time;
	const float interpFraction = FMath::Clamp((HitTime - OlderFrame.time) / distance, 0, 1);

	FRollbackFrame interpFramePackage;
	interpFramePackage.time = HitTime;
	interpFramePackage.halfHeight = NewerFrame.halfHeight;
	interpFramePackage.radius = NewerFrame.radius;
	interpFramePackage.hitboxLocation = FMath::VInterpTo(OlderFrame.hitboxLocation, NewerFrame.hitboxLocation, 1.f, interpFraction);
	
	return interpFramePackage;
}

FServerSideRewindResult URollbackComponent::ConfirmHit(const FRollbackFrame& FrameData, APawn* HitCharacter,
	const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation)
{
	if (!IsValid(HitCharacter) || !HitCharacter->Implements<UCommonUtility>())
	{
		return FServerSideRewindResult();
	}
	
	FRollbackFrame cachedFrame;
	CacheHitBoxPositions(HitCharacter, cachedFrame);
	MoveHitBoxes(HitCharacter, FrameData, true);
	ICommonUtility::Execute_ToggleMainCollision(HitCharacter, false);
	
	const FVector traceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	FHitResult hitResult;
	if (IsValid(GetWorld()))
	{
		GetWorld()->LineTraceSingleByChannel(hitResult, TraceStart, traceEnd, ECollisionChannel::ECC_Visibility);
		if (hitResult.bBlockingHit)
		{
			MoveHitBoxes(HitCharacter, cachedFrame, false);
			ICommonUtility::Execute_ToggleMainCollision(HitCharacter, true);
			return FServerSideRewindResult{true};
		}
	}

	MoveHitBoxes(HitCharacter, cachedFrame, false);
	ICommonUtility::Execute_ToggleMainCollision(HitCharacter, true);
	return FServerSideRewindResult{false};
}

void URollbackComponent::CacheHitBoxPositions(APawn* HitCharacter, FRollbackFrame& OutRollbackFrame)
{
	if (!HitCharacter->Implements<UCommonUtility>())
	{
		return;
	}
	
	UCapsuleComponent* capsuleComponent = ICommonUtility::Execute_GetHitBoxCollisionComponent(HitCharacter);
	if (!IsValid(HitCharacter) || !IsValid(capsuleComponent))
	{
		return;
	}
	
	OutRollbackFrame.hitboxLocation = capsuleComponent->GetComponentLocation();
	OutRollbackFrame.halfHeight = capsuleComponent->GetScaledCapsuleHalfHeight();
	OutRollbackFrame.radius = capsuleComponent->GetScaledCapsuleRadius();
}

void URollbackComponent::MoveHitBoxes(APawn* HitCharacter, const FRollbackFrame& FramePackage, bool EnableCollision)
{
	if (!HitCharacter->Implements<UCommonUtility>())
	{
		return;
	}
	
	UCapsuleComponent* capsuleComponent = ICommonUtility::Execute_GetHitBoxCollisionComponent(HitCharacter);
	if (!IsValid(HitCharacter) || !IsValid(capsuleComponent))
	{
		return;
	}
	
	capsuleComponent->SetWorldLocation(FramePackage.hitboxLocation);
	capsuleComponent->SetCapsuleHalfHeight(FramePackage.halfHeight);
	capsuleComponent->SetCapsuleRadius(FramePackage.radius);
	capsuleComponent->SetCollisionEnabled(EnableCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	capsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void URollbackComponent::ProcessFrameHistory()
{
	if (!IsValid(GetOwner()) || !UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}

	FRollbackFrame newFrame;
	if (_FrameHistory.Num() <= 1)
	{
		SaveFrame(newFrame);
		_FrameHistory.AddHead(newFrame);
	}
	else
	{
		float historyLength = _FrameHistory.GetHead()->GetValue().time - _FrameHistory.GetTail()->GetValue().time;
		while (historyLength > _MaxFrameRecordTime)
		{
			_FrameHistory.RemoveNode(_FrameHistory.GetTail());
			historyLength = _FrameHistory.GetHead()->GetValue().time - _FrameHistory.GetTail()->GetValue().time;
		}

		SaveFrame(newFrame);
		_FrameHistory.AddHead(newFrame);
	}

	if (_DrawDebugHitbox)
	{
		ShowFrame(newFrame);
	}
}

void URollbackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URollbackComponent::ShowFrame(const FRollbackFrame& Frame)
{
	if (!IsValid(GetWorld()))
	{
		return;
	}
	FQuat rotator = FQuat::Identity;
	DrawDebugCapsule(GetWorld(), Frame.hitboxLocation, Frame.halfHeight, Frame.radius, rotator, FColor::Orange, false, _MaxFrameRecordTime);
}

void URollbackComponent::SaveFrame(FRollbackFrame& Frame)
{
	if (!IsValid(GetOwner()) || !GetOwner()->Implements<UCommonUtility>())
	{
		return;
	}
	
	UCapsuleComponent* capsuleComponent = ICommonUtility::Execute_GetHitBoxCollisionComponent(GetOwner());
	if (!IsValid(capsuleComponent))
	{
		return;
	}
	
	Frame.time = GetWorld()->GetTimeSeconds();
	Frame.hitboxLocation = capsuleComponent->GetComponentLocation();
	Frame.halfHeight = capsuleComponent->GetScaledCapsuleHalfHeight();
	Frame.radius = capsuleComponent->GetScaledCapsuleRadius();
}
