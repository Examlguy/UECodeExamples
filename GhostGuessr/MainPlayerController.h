// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMicrophoneVolume, float, VolumeLevel);

UCLASS()
class GHOSTGUESSR_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* InPawn) override;
	
	void SetupPossessableData(APawn* InPawn);
	void RemovePossessableData();
	
	UFUNCTION(Server, Reliable)
	void Server_RequestPossessPawn(APawn* InPawn);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MainPlayerController")
	void OnControllerChanged(EHardwareDevicePrimaryType DeviceType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MainPlayerController")
	void OnPossessionChanged();
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FMicrophoneVolume MicrophoneVolumeDelegate;

protected:
	UFUNCTION()
	void OnInputHardwareChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);
	
	UPROPERTY(EditDefaultsOnly, Category = Default)
	TObjectPtr<UInputMappingContext> _MainControllerControls;

	UPROPERTY(BlueprintReadOnly, Category = Default)
	EHardwareDevicePrimaryType _CurrentControlType;
	
	UPROPERTY(BlueprintReadOnly, Category = Default)
	TObjectPtr<UUserWidget> _CurrentControlledHUD;
	
	UPROPERTY()
	TArray<TObjectPtr<UInputMappingContext>> _MappingContextsInUse;
};

