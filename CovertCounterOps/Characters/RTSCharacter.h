// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedSightTeamComponent.h"
#include "GMCPawn.h"
#include "SelectionManagerComponent.h"
#include "Interfaces/CommonUtility.h"
#include "Interfaces/Possessable.h"
#include "RTSCharacter.generated.h"

USTRUCT(BlueprintType)
struct FPawnMoveCommand
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector_NetQuantize commandLocation;
	UPROPERTY()
	TObjectPtr<APawn> commandedPawn = nullptr;
	UPROPERTY()
	bool isAltAction = false;
	UPROPERTY()
	bool hasLookLocation = false;
	UPROPERTY()
	FVector_NetQuantize lookLocation;
};

class AStaticMeshActor;

UCLASS()
class COVERTCOUNTEROPS_API ARTSCharacter : public AGMC_Pawn, public IPossessable, public ICommonUtility
{
	GENERATED_BODY()
public:
	ARTSCharacter();
	
	UFUNCTION(BlueprintCallable)
	void SetClickLocation(FVector ClickLocation) { _LastClickedLocation = ClickLocation; }
	
	UFUNCTION(BlueprintCallable)
	bool IsMouseFarFromClickedLocation(FHitResult NewHit, FHitResult OldHit);

	virtual TSubclassOf<UUserWidget> GetPossessableHUD_Implementation() override { return _RTSHUD; }

	virtual void SetTeam_Implementation(int32 TeamNumber) override;
	virtual int32 GetTeam_Implementation() override { return _TeamNumber; }

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	FHitResult SelectOnSurface();
	
	UFUNCTION(BlueprintCallable)
	FHitResult SelectThroughWall();

	UFUNCTION(BlueprintCallable)
	FHitResult SelectToFirstVisibleSurface(UMaterialInterface* MaterialToIgnore = nullptr);
	
	UFUNCTION(BlueprintCallable)
	void ContextSelectPawnAction(APawn* SelectedPawn);

	UFUNCTION(BlueprintCallable)
	void IssueMoveCommandToPawns(FHitResult CommandLocation, bool HasLookLocation, FVector LookLocation);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_IssueMoveCommandToPawn(FPawnMoveCommand MoveCommand);
	
	UFUNCTION(BlueprintCallable)
	void AlternateActionDesired(bool IsHeld) { _AlternateActionDesired = IsHeld; }
	
	UFUNCTION(BlueprintCallable)
	bool TryPossessCharacter(APawn* TargetPawn);
	
	UFUNCTION(BlueprintCallable)
	bool SelectPawn(APawn* TargetPawn);

	UFUNCTION(BlueprintCallable)
	void DeselectPawn(APawn* TargetPawn);

	UFUNCTION(BlueprintCallable)
	void DeselectAllPawns();

	UFUNCTION()
	void SetupSelectableCharacters();
	
	UFUNCTION()
	void OnRep_TeamNumberChanged();

	UFUNCTION()
	void OnActorSelected(AActor* SelectedActor, bool IsSelected);
	
protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	
	UPROPERTY(ReplicatedUsing=OnRep_TeamNumberChanged)
	int32 _TeamNumber;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAdvancedSightTeamComponent> _TeamComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USelectionManagerComponent> _SelectionManagerComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> _RTSHUD;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APawn>> _SelectablePawns;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<APawn*> _SelectedCommandablePawns;

	UPROPERTY(BlueprintReadWrite)
	bool _AlternateActionDesired = false;
	
	FVector _LastClickedLocation;
};
