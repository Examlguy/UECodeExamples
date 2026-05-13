// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "Interfaces/PossessableInterface.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(_MainControllerControls, 999);
	}
	
	if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		InputDeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &AMainPlayerController::OnInputHardwareChanged);
		_CurrentControlType = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetLocalPlayer()->GetPlatformUserId()).PrimaryDeviceType;
		OnControllerChanged(_CurrentControlType);
	}
}

void AMainPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	if (IsLocalController())
	{
		SetupPossessableData(InPawn);
		OnPossessionChanged();
	}
}

void AMainPlayerController::SetupPossessableData(APawn* InPawn)
{
	RemovePossessableData();
	if (InPawn->Implements<UPossessableInterface>())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (UInputMappingContext* inputMapping = IPossessableInterface::Execute_GetControlMapping(InPawn))
			{
				Subsystem->AddMappingContext(inputMapping, 0);
				_MappingContextsInUse.Add(inputMapping);
			}
		}

		if (TSubclassOf<UUserWidget> pawnHUD = IPossessableInterface::Execute_GetPossessableHUD(InPawn))
		{
			_CurrentControlledHUD = CreateWidget(this, pawnHUD);
			if (IsValid(_CurrentControlledHUD))
			{
				_CurrentControlledHUD->AddToViewport();
			}
		}
	}
}

void AMainPlayerController::RemovePossessableData()
{
	for (UInputMappingContext* mappingContext : _MappingContextsInUse)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(mappingContext);
		}
	}
	_MappingContextsInUse.Empty();

	if (IsValid(_CurrentControlledHUD))
	{
		_CurrentControlledHUD->RemoveFromParent();
	}
}

void AMainPlayerController::OnInputHardwareChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	if (UInputDeviceSubsystem* InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>())
	{
		_CurrentControlType = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(GetLocalPlayer()->GetPlatformUserId()).PrimaryDeviceType;
		OnControllerChanged(_CurrentControlType);
	}
}

void AMainPlayerController::Server_RequestPossessPawn_Implementation(APawn* InPawn)
{
	if (IsValid(InPawn->GetController<AMainPlayerController>()))
	{
		return;
	}

	if (!InPawn->Implements<UPossessableInterface>())
	{
		return;
	}

	if (IPossessableInterface::Execute_CanBePossessedBy(InPawn, this))
	{
		bool HasDefaultAIController = IsValid(GetPawn()->AIControllerClass);
		AController* currentPawnDefaultController = HasDefaultAIController ? Cast<AController>(GetWorld()->SpawnActor(GetPawn()->AIControllerClass)) : nullptr;
		APawn* oldPawn = GetPawn();
		
		UnPossess();
		
		if (HasDefaultAIController && IsValid(currentPawnDefaultController))
		{
			currentPawnDefaultController->Possess(oldPawn);
		}
		
		if (AController* newController = InPawn->Controller)
		{
			newController->UnPossess();
			newController->Destroy();
		}
		
		Possess(InPawn);
	}
}
