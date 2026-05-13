// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GMCPlayerController.h"
#include "InputMappingContext.h"
#include "MainPlayerController.generated.h"

UCLASS()
class COVERTCOUNTEROPS_API AMainPlayerController : public AGMC_PlayerController
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* PlayerControllerMappingContext;

public:
	AMainPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void ReceivedPlayer() override;
	
	UFUNCTION(BlueprintCallable)
	int32 GetControllerTeam() const;

	void SetupPossessableData(APawn* InPawn);
	void RemovePossessableData();

	virtual void AcknowledgePossession(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable)
	void ShowWhatRTSCantSee(bool IsVisible);
	
	UFUNCTION(BlueprintCallable)
	void SetMainControlledPawn(APawn* InPawn);
	
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_RequestPossessPawn(APawn* InPawn);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestPossessPawn(APawn* InPawn);

	float _SingleTripTime = 0.f;
	
protected:
	// Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void Server_RequestServerTime(float TimeOfClientRequest);

	// Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void Client_ReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	TObjectPtr<APawn> _MainControlledPawn;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> _CurrentControlledHUD;
	
	UPROPERTY()
	TArray<UInputMappingContext*> _MappingContextsInUse;
};
