#include "MarcoCharacter.h"
#include "HighResScreenshot.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UE5Coro/Coroutine.h"
#include "UE5Coro/LatentAwaiters.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/World/AreaSpawnNode.h"
#include "WhereIsMarco/World/Clue.h"
#include "WhereIsMarco/World/WhereIsMarcoWorldSettings.h"

// Test screenshot functionality in standalone builds
// The PIE likes to fuck with the screenshots for some thumbnails or something

AMarcoCharacter::AMarcoCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMarcoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMarcoCharacter::ClearClues()
{
	for (AActor* clueOrObject : _cluesAndObjectsSpawned)
	{
		if (IsValid(clueOrObject))
		{
			clueOrObject->Destroy();
		}
	}
	_cluesAndObjectsSpawned.Empty();
}

void AMarcoCharacter::GenerateMarcoLocations(int locations_to_gen)
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()) || _isBusy || locations_to_gen == 0)
	{
		return;
	}
	
	_isBusy = true;
	_numberLocationsToGenerate = locations_to_gen;
	
	ClearClues();

	AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>();
	if (IsValid(gameState))
	{
		UStageConfigurationDataAsset* stageConfig = gameState->GetStageConfigurationDataAsset();
		if (IsValid(stageConfig))
		{
			UGameInstance* gameInstance = GetGameInstance();
			UGameConfigurationInstanceSubsystem* gameConfig = gameInstance->GetSubsystem<UGameConfigurationInstanceSubsystem>();

			for (FPlayModeDataSet stageData :  stageConfig->_playModeSetup)
			{
				if (stageData.playMode == gameConfig->GetPlayMode())
				{
					_objectsToSpawn = stageData.stageObjectsToSpawn;
					break;
				}
			}
		}
	}
	GetWorld()->GetFirstPlayerController()->SetViewTarget(_cameraActor);

	_spawnLocations.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAreaSpawnNode::StaticClass(), _spawnLocations);

	if (_spawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No spawn locations to use"));
	}
	
	GenerateScreenshotAndLocation();
}

void AMarcoCharacter::OnScreenshotCaptured(int32 width, int32 height, const TArray<FColor>& image_data)
{
	UE_LOG(LogTemp, Warning, TEXT("### SCREENSHOT CAPTURED ###"));

	if (IsValid(_flashComponent))
	{
		_flashComponent->SetHiddenInGame(true);
	}

	UTexture2D* texture = UTexture2D::CreateTransient(width, height, PF_B8G8R8A8);

	void* RawData = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(RawData, image_data.GetData(), width * height * sizeof(FColor));
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	texture->CompressionSettings = TC_VectorDisplacementmap;
#if WITH_EDITOR
	texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	texture->UpdateResource();

	AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>();
	if (IsValid(gameState))
	{
		if (UNavigationSystemV1* navigation = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			AWhereIsMarcoWorldSettings* worldSettings = Cast<AWhereIsMarcoWorldSettings>(GetWorldSettings());
			TArray<AActor*> spawnPoints = worldSettings->GetRespawnLocations();
			
			FVector startLocation = spawnPoints[0]->GetActorLocation();
			FVector endLocation = GetActorLocation();
			UNavigationPath* path = navigation->FindPathToLocationSynchronously(GetWorld(), startLocation, endLocation);

			TArray<TObjectPtr<AActor>> actorsToIgnore;
			FHitResult hitResult;
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), path->PathPoints[path->PathPoints.Num() - 1], path->PathPoints[path->PathPoints.Num() - 1] + GetActorUpVector() * -500, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::None,hitResult, true);
			
			FVector location = hitResult.bBlockingHit ? hitResult.Location : FVector(path->PathPoints[path->PathPoints.Num() - 1].X, path->PathPoints[path->PathPoints.Num() - 1].Y, path->PathPoints[path->PathPoints.Num() - 1].Z - GetEyeHeightOffset().Z);
			FRotator rotation = GetActorRotation();
			rotation.Pitch = 0.0f;
		
			if (gameState->GetMarcoLocationsNum() == _objectsToSpawn.Num() - 1)
			{
				AActor* objectSpawned = GetWorld()->SpawnActor(_objectsToSpawn[gameState->GetMarcoLocationsNum()], &location, &rotation);
				_cluesAndObjectsSpawned.Add(objectSpawned);
				navigation->Build();
			}

			path = navigation->FindPathToLocationSynchronously(GetWorld(), startLocation, location);
			
			if (IsValid(path) && path->IsValid() && path->PathPoints.Num() > 0)
			{
				if (path->PathPoints[path->PathPoints.Num() - 1] == FVector::ZeroVector)
				{
					UE_LOG(LogTemp, Warning, TEXT("! Area Node (%s) is broken"), *GetName());
				}

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), path->PathPoints[path->PathPoints.Num() - 1], GetActorLocation() + GetActorUpVector() * -500, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::None,hitResult, true);
				
				location = hitResult.Location;
			}

			AClue* clue = Cast<AClue>(GetWorld()->SpawnActor(_clueClass, &location, &rotation));
			if (IsValid(clue))
			{
				if (gameState->GetMarcoLocationsNum() + 1 == _numberLocationsToGenerate)
				{
					clue->SetIsFinalClue(true);
				}
				_cluesAndObjectsSpawned.Add(clue);
			}
			
			gameState->AddMarcoLocationScreenshot(gameState->GetMarcoLocationsNum(), texture, GetActorLocation());
		}
	}
}

void AMarcoCharacter::OnScreenshotCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("### SCREENSHOT COMPLETED ###"));

	AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>();
	if (IsValid(gameState))
	{
		if (_numberLocationsToGenerate - gameState->GetMarcoLocationsNum() > 0)
		{
			GenerateScreenshotAndLocation();
		}
		else
		{
			GetWorld()->GetFirstPlayerController()->SetViewTarget(GetWorld()->GetFirstPlayerController()->GetPawn());
			_isBusy = false;
		}
	}
}

void AMarcoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		return;
	}
	
	GEngine->GameViewport->OnScreenshotCaptured().AddUObject(this, &AMarcoCharacter::OnScreenshotCaptured);
	FScreenshotRequest::OnScreenshotRequestProcessed().AddUObject(this, &AMarcoCharacter::OnScreenshotCompleted);
	
	_cameraActor = Cast<ACameraActor>(GetWorld()->SpawnActor(ACameraActor::StaticClass()));
	if (IsValid(_cameraActor))
	{
		_cameraActor->GetCameraComponent()->SetFieldOfView(60);
		_cameraActor->GetCameraComponent()->SetAspectRatio(_widthResolution / _heightResolution);
	}
}

UE5Coro::TCoroutine<> AMarcoCharacter::GenerateScreenshotAndLocation(FForceLatentCoroutine)
{
	if (!UKismetSystemLibrary::IsServer(GetWorld()))
	{
		co_return;
	}
	
	if (_spawnLocations.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Ran out of spawn locations, refilling array."));
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAreaSpawnNode::StaticClass(), _spawnLocations);
	}
	
	FVector nav_location;
	int locationIndex = FMath::RandRange(0, _spawnLocations.Num() - 1);
	AAreaSpawnNode* spawnLocation = Cast<AAreaSpawnNode>(_spawnLocations[locationIndex]);
	if (IsValid(spawnLocation))
	{
		nav_location = spawnLocation->GetRandomLocationInNavigatableArea();
		_spawnLocations.RemoveAt(locationIndex);

		AWhereIsMarcoGameMode* gameMode = Cast<AWhereIsMarcoGameMode>(GetWorld()->GetAuthGameMode());
		UDifficultyDataAsset* difficultyData = gameMode->GetDifficultyData();

		if (IsValid(gameMode) && IsValid(difficultyData))
		{
			FClueDifficulty clueDifficulty = difficultyData->GetClueDifficultyAtClueNumber(gameMode->GetGameStage());

			SetActorLocation(FVector(nav_location.X, nav_location.Y, nav_location.Z + _capsuleComponent->GetScaledCapsuleHalfHeight()));
			SetActorRotation(FRotator(UKismetMathLibrary::RandomFloatInRange(clueDifficulty._marcoLookDownRange,clueDifficulty._marcoLookUpRange),UKismetMathLibrary::RandomFloatInRange(0,360),GetActorRotation().Roll));

			DrawDebugSphere(GetWorld(), GetActorLocation(), 50, 8, FColor::Yellow, true);

			if (IsValid(_flashComponent))
			{
				_flashComponent->SetHiddenInGame(false);
			}
			
			if (IsValid(_cameraActor))
			{
				_cameraActor->SetActorLocation(GetPawnViewLocation());
				_cameraActor->SetActorRotation(GetActorRotation());
			
				// Give camera time to render
				co_await UE5Coro::Latent::NextTick();
				
				GetHighResScreenshotConfig().SetResolution(_widthResolution, _heightResolution);
				FScreenshotRequest::RequestScreenshot(false);
			}
		}
	}
}