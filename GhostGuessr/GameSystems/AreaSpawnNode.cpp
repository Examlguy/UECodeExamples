#include "AreaSpawnNode.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GhostGuessr/GhostGuessrWorldSettings.h"
#include "Kismet/GameplayStatics.h"

AAreaSpawnNode::AAreaSpawnNode()
{
	PrimaryActorTick.bCanEverTick = false;
	_BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	_BoxComponent->SetupAttachment(RootComponent);
}

void AAreaSpawnNode::BeginPlay()
{
	Super::BeginPlay();
}

void AAreaSpawnNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AAreaSpawnNode::GetRandomLocationInNavigatableArea()
{
	TArray<TObjectPtr<AActor>> actorsToIgnore;
	FHitResult hitResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * -500, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, actorsToIgnore, EDrawDebugTrace::None,hitResult, true);
	
	float groundPosition = hitResult.bBlockingHit ? hitResult.Location.Z : GetActorLocation().Z;
	FVector randomVector(FMath::FRandRange(-_BoxComponent->GetScaledBoxExtent().X,_BoxComponent->GetScaledBoxExtent().X),FMath::FRandRange(-_BoxComponent->GetScaledBoxExtent().Y,_BoxComponent->GetScaledBoxExtent().Y), groundPosition);

	randomVector = randomVector.RotateAngleAxis(GetActorRotation().Yaw,FVector(0,0,1));
	randomVector.X += GetActorLocation().X;
	randomVector.Y += GetActorLocation().Y;
	
	DrawDebugSphere(GetWorld(), randomVector, 50, 8, FColor::Red, true);
	
	if (UNavigationSystemV1* navigation = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		AGhostGuessrWorldSettings* worldSettings = Cast<AGhostGuessrWorldSettings>(GetWorldSettings());
		TArray<AActor*> spawnPoints = worldSettings->GetPlayerSpawnLocations();
		
		FVector startLocation = spawnPoints[0]->GetActorLocation();
		FVector endLocation = randomVector;
		
		UNavigationPath* path = navigation->FindPathToLocationSynchronously(GetWorld(), startLocation, endLocation);

		for (auto point : path->PathPoints)
		{
			DrawDebugSphere(GetWorld(), point, 25, 4, FColor::Yellow, true);
		}
		
		if (IsValid(path) && path->IsValid() && path->PathPoints.Num() > 0)
		{
			if (path->PathPoints[path->PathPoints.Num() - 1] == FVector::ZeroVector)
			{
				UE_LOG(LogTemp, Warning, TEXT("! Area Node (%s) is broken"), *GetName())
			}
			return path->PathPoints[path->PathPoints.Num() - 1];
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("!! Area Node (%s) is broken"), *GetName())
	return FVector::ZeroVector;
}

