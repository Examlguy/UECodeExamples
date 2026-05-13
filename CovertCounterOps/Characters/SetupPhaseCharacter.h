// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSCharacter.h"
#include "CovertCounterOps/Game/A1VXGameState.h"
#include "SetupPhaseCharacter.generated.h"

UCLASS()
class COVERTCOUNTEROPS_API ASetupPhaseCharacter : public ARTSCharacter
{
	GENERATED_BODY()

public:
	ASetupPhaseCharacter();
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void TrySetDesiredSpawnPoint(FVector SpawnLocation);
	void PredictedSetDesiredSpawnPoint(FVector SpawnLocation);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SetDesiredSpawnPoint(FVector_NetQuantize SpawnLocation);

	UFUNCTION(BlueprintNativeEvent)
	bool IsValidSpawnLocation(ETeam ValidForTeam, FVector SpawnLocation);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestSpawnActor(UClass* ActorClass, FVector_NetQuantize SpawnLocation, FVector_NetQuantize SpawnNormal);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_RequestDestroyActor(AActor* ActorToDestroy);
	
	UFUNCTION(BlueprintCallable)
	void DestroySelectedActors();
	
	void ContextSelectPawnAction() = delete;
	void ContextIssueCommandToPawnsAction() = delete;
	bool TryPossessCharacter(APawn* TargetPawn) = delete;
	bool SelectPawn(APawn* TargetPawn) = delete;
	void DeselectPawn(APawn* TargetPawn) = delete;
	void DeselectAllPawns() = delete;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> _SelectedActors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int _TotalBuildPoints = 40;
	UPROPERTY(Replicated, BlueprintReadOnly)
	int _CurrentBuildPoints;
};
