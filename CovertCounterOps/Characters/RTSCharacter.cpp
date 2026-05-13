#include "RTSCharacter.h"

#include "SelectionManagerComponent.h"
#include "SoldierCharacter.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Net/UnrealNetwork.h"

ARTSCharacter::ARTSCharacter()
{
	_TeamComponent = CreateDefaultSubobject<UAdvancedSightTeamComponent>(TEXT("TeamComponent"));
	_TeamNumber = static_cast<int32>(ETeam::DEFENDER);
	_TeamComponent->TeamID = _TeamNumber;
}

bool ARTSCharacter::IsMouseFarFromClickedLocation(FHitResult NewHit, FHitResult OldHit)
{
	if (NewHit.bBlockingHit)
	{
		return FVector::Distance(NewHit.Location, OldHit.Location) > 150.0f;
	}

	return false;
}

void ARTSCharacter::SetTeam_Implementation(int32 TeamNumber)
{
	_TeamNumber = TeamNumber;
	OnRep_TeamNumberChanged();
}

void ARTSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARTSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARTSCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSCharacter, _TeamNumber);
}

FHitResult ARTSCharacter::SelectOnSurface()
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return FHitResult();
	}
	
	FHitResult Hit;
	PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
	
	return Hit;
}

FHitResult ARTSCharacter::SelectThroughWall()
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return FHitResult();
	}
	
	FVector mouseWorldLocation;
	FVector mouseWorldDirection;
	PlayerController->DeprojectMousePositionToWorld(mouseWorldLocation, mouseWorldDirection);

	TArray<AActor*> actorsToIgnore;
	
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return FHitResult();
	}
	
	for (auto player : gameState->GetAttackerPlayers())
	{
		actorsToIgnore.Add(player->GetPawn());
	}
	
	FHitResult Hit;
	UKismetSystemLibrary::BoxTraceSingle(GetWorld(), mouseWorldLocation, mouseWorldDirection * 100000 + mouseWorldLocation, FVector(50,50,50), GetActorRotation(), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel3), false, actorsToIgnore, EDrawDebugTrace::ForDuration, Hit, true);

	return Hit;
}

FHitResult ARTSCharacter::SelectToFirstVisibleSurface(UMaterialInterface* MaterialToIgnore)
{
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return FHitResult();
	}
	
	FVector mouseWorldLocation;
	FVector mouseWorldDirection;
	PlayerController->DeprojectMousePositionToWorld(mouseWorldLocation, mouseWorldDirection);

	TArray<AActor*> actorsToIgnore;
	
	A1VXGameState* gameState = GetWorld()->GetGameState<A1VXGameState>();
	if (!IsValid(gameState))
	{
		return FHitResult();
	}
	
	for (auto player : gameState->GetAttackerPlayers())
	{
		actorsToIgnore.Add(player->GetPawn());
	}
	
	FHitResult hit;
	hit.ImpactPoint = mouseWorldLocation;
	for (int i = 0; i < 20; ++i)
	{
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), hit.ImpactPoint, mouseWorldDirection * 100000 + hit.ImpactPoint, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), true, actorsToIgnore, EDrawDebugTrace::ForDuration, hit, true);

		if (!IsValid(MaterialToIgnore))
		{
			return hit;
		}

		if (!hit.bBlockingHit)
		{
			break;
		}

		if (hit.GetActor()->IsHidden())
		{
			actorsToIgnore.Add(hit.GetActor());
			continue;
		}
		
		if (AStaticMeshActor* mesh = Cast<AStaticMeshActor>(hit.GetActor()))
		{
			if (UStaticMeshComponent* component = mesh->GetStaticMeshComponent())
			{
				int32 section;
				UMaterialInterface* materialHit = component->GetMaterialFromCollisionFaceIndex(hit.FaceIndex, section);
				if (!IsValid(materialHit))
				{
					actorsToIgnore.Add(hit.GetActor());
					continue;
				}
				
				UE_LOG(LogTemp, Display, TEXT("MaterialHit: %s"), *materialHit->GetName());
				if (materialHit != MaterialToIgnore)
				{
					UE_LOG(LogTemp, Display, TEXT("MaterialHit Returned: %s"), *materialHit->GetName());
					return hit;
				}
			}
		}
		
		actorsToIgnore.Add(hit.GetActor());
	}
	
	return FHitResult();
}

void ARTSCharacter::ContextSelectPawnAction(APawn* SelectedPawn)
{
	if (!IsValid(SelectedPawn))
	{
		if (!_AlternateActionDesired)
		{
			DeselectAllPawns();
		}
	}
	
	AMainPlayerController* PlayerController = Cast<AMainPlayerController>(GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	if (!_AlternateActionDesired)
	{
		if (!TryPossessCharacter(SelectedPawn))
		{
			DeselectAllPawns();
			SelectPawn(SelectedPawn);
		}
	}
	else
	{
		SelectPawn(SelectedPawn);
	}

	UE_LOG(LogTemp, Warning, TEXT("Actors Selected:"))
	for (auto actor : _SelectedCommandablePawns)
	{
		UE_LOG(LogTemp, Warning, TEXT("- %s"), *actor->GetName())
	}
}

void ARTSCharacter::IssueMoveCommandToPawns(FHitResult CommandLocation, bool HasLookLocation, FVector LookLocation)
{
	FPawnMoveCommand newMoveCommand;
	newMoveCommand.commandLocation = CommandLocation.Location;
	newMoveCommand.isAltAction = _AlternateActionDesired;
	newMoveCommand.hasLookLocation = HasLookLocation;
	newMoveCommand.lookLocation = LookLocation;
	
	for (APawn* selectedPawn : _SelectedCommandablePawns)
	{
		newMoveCommand.commandedPawn = selectedPawn;
		Server_IssueMoveCommandToPawn(newMoveCommand);
	}
}

void ARTSCharacter::Server_IssueMoveCommandToPawn_Implementation(FPawnMoveCommand MoveCommand)
{
	if (ASoldierCharacter* soldier = Cast<ASoldierCharacter>(MoveCommand.commandedPawn))
	{
		if (UAIUtilityComponent* utility = IAIUtility::Execute_GetAIUtilityComponent(soldier))
		{
			utility->AddPatrolPoint(MoveCommand.commandLocation, _AlternateActionDesired);
			utility->SetFocusRequest(MoveCommand.lookLocation, MoveCommand.hasLookLocation);
		}
	}
}

bool ARTSCharacter::TryPossessCharacter(APawn* TargetPawn)
{
	if (_AlternateActionDesired || !TargetPawn->Implements<UPossessable>() || !IsValid(GetController()))
	{
		return false;
	}

	AMainPlayerController* PlayerController = GetController<AMainPlayerController>();
	if (IPossessable::Execute_CanBePossessedBy(TargetPawn, PlayerController) && IsValid(PlayerController))
	{
		PlayerController->Client_RequestPossessPawn(TargetPawn);
		return true;
	}
	
	return false;
}

bool ARTSCharacter::SelectPawn(APawn* TargetPawn)
{
	if (_SelectedCommandablePawns.Find(TargetPawn) == INDEX_NONE)
	{
		_SelectedCommandablePawns.Add(TargetPawn);
		return true;
	}
	
	return false;
}

void ARTSCharacter::DeselectPawn(APawn* TargetPawn)
{
	int TargetPawnIndex = _SelectedCommandablePawns.Find(TargetPawn);
	if (TargetPawnIndex != INDEX_NONE)
	{
		_SelectedCommandablePawns.RemoveAt(TargetPawnIndex);
	}
}

void ARTSCharacter::DeselectAllPawns()
{
	_SelectedCommandablePawns.Empty();
}

void ARTSCharacter::SetupSelectableCharacters()
{
	if (!IsValid(_SelectionManagerComponent))
	{
		return;
	}
	
	for (TSubclassOf<APawn> subClass : _SelectablePawns)
	{
		TArray<AActor*> allActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), subClass, allActors);
		FObjectSelectionSettings selectionSettings;
		selectionSettings.CanBeSelectedByIndex = true;
		selectionSettings.CanBeSelectedViaSelectAll = true;
		selectionSettings.CanBeSelectedWithMarquee = true;
		selectionSettings.UseSimpleCollision = true;
		
		for (auto actor : allActors)
		{
			_SelectionManagerComponent->RegisterObject(actor, nullptr, selectionSettings);
			if (actor->Implements<UAIUtility>()  && actor->Implements<UCommonUtility>())
			{
				if (_TeamNumber == ICommonUtility::Execute_GetTeam(actor))
				{
					UAIUtilityComponent* component = IAIUtility::Execute_GetAIUtilityComponent(actor);
					if (IsValid(component))
					{
						component->OnActorSelectedDelegate.AddUniqueDynamic(this, &ARTSCharacter::OnActorSelected);
					}
				}
			}
		}
	}
}

void ARTSCharacter::OnRep_TeamNumberChanged()
{
	_TeamComponent->SwitchTeam(_TeamNumber);
}

void ARTSCharacter::BeginPlay()
{
	Super::BeginPlay();
	_SelectionManagerComponent = FindComponentByClass<USelectionManagerComponent>();
	SetupSelectableCharacters();
}

void ARTSCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ARTSCharacter::OnActorSelected(AActor* SelectedActor, bool IsSelected)
{
	if (IsSelected)
	{
		if (APawn* pawn = Cast<APawn>(SelectedActor))
		{
			SelectPawn(pawn);
		}
	}
	else
	{
		if (APawn* pawn = Cast<APawn>(SelectedActor))
		{
			DeselectPawn(pawn);
		}
	}
}
