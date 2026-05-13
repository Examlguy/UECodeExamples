// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "Interfaces/Possessable.h"
#include "SpectatorCharacter.generated.h"

UCLASS()
class COVERTCOUNTEROPS_API ASpectatorCharacter : public ASpectatorPawn, public IPossessable
{
	GENERATED_BODY()

public:
	ASpectatorCharacter();

	virtual TSubclassOf<UUserWidget> GetPossessableHUD_Implementation() override { return _SpectatorHUD; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> _SpectatorHUD;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
