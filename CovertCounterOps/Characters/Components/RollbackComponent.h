// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RollbackComponent.generated.h"

USTRUCT(BlueprintType)
struct FRollbackFrame
{
	GENERATED_BODY()

	UPROPERTY()
	FVector hitboxLocation;

	UPROPERTY()
	float halfHeight = 1;

	UPROPERTY()
	float radius = 1;
	
	UPROPERTY()
	float time = -1;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool hitConfirmed = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COVERTCOUNTEROPS_API URollbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URollbackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void ShowFrame(const FRollbackFrame& Frame);
	
	void SaveFrame(FRollbackFrame& Frame);

	UFUNCTION(BlueprintCallable)
	FServerSideRewindResult ServerSideRewind(APawn* HitCharacter, const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation, float HitTime);
	
protected:
	virtual void BeginPlay() override;
	FRollbackFrame InterpBetweenFrames(const FRollbackFrame& OlderFrame, const FRollbackFrame& NewerFrame, float HitTime);
	FServerSideRewindResult ConfirmHit(const FRollbackFrame& FrameData, APawn* HitCharacter, const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation);
	void CacheHitBoxPositions(APawn* HitCharacter, FRollbackFrame& OutRollbackFrame);
	void MoveHitBoxes(APawn* HitCharacter, const FRollbackFrame& FramePackage, bool EnableCollision);
	void ProcessFrameHistory();
private:
	TDoubleLinkedList<FRollbackFrame> _FrameHistory;
	
	UPROPERTY(EditAnywhere)
	float _MaxFrameRecordTime = 4.f;
	
	UPROPERTY(EditAnywhere)
	bool _DrawDebugHitbox = false;
};

