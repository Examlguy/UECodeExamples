#include "Gun.h"

#include "CovertCounterOps/Characters/Components/RollbackComponent.h"
#include "CovertCounterOps/Characters/Interfaces/CommonUtility.h"
#include "CovertCounterOps/Controllers/MainPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

AGun::AGun()
{
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	_TimeBetweenShots = 60 / _RoundsPerMinute;
	_RandomStream = FRandomStream();
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float newFireTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	if (IsValid(GetOwner()) && _IsUsing && newFireTime - _LastFireTime >= _TimeBetweenShots)
	{
		Fire();
		_LastFireTime = newFireTime;
	}
}

void AGun::Fire()
{
	FireBullet();
}

void AGun::FireBullet()
{
	FVector spreadOffset = (GetFireDirectionComponent()->GetRightVector() * sin(_RandomStream.FRandRange(-1, 1)) * _MaxSpreadDistance) + (GetFireDirectionComponent()->GetUpVector() * sin(_RandomStream.FRandRange(-1, 1)) * _MaxSpreadDistance);
	FVector spreadEnd = GetShootingDirection(_Distance) + spreadOffset;

	FVector shotDirection = spreadEnd - GetActorLocation();
	OnGunFiredDelegate.Broadcast(shotDirection);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	FVector TraceStart = GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, GetActorLocation() + shotDirection * 10000, ECollisionChannel::ECC_GameTraceChannel1, Params);

	if (!Hit.bBlockingHit)
	{
		return;
	}

	if (APawn* hitCharacter = Cast<APawn>(Hit.GetActor()))
	{
		if (APawn* ownerPawn = Cast<APawn>(GetOwner()))
		{
			if (HasAuthority() && ownerPawn->IsLocallyControlled())
			{
				UE_LOG(LogTemp, Warning, TEXT("Applying Damage from Server"));
				if (AController* controller = hitCharacter->GetController())
				{
					UGameplayStatics::ApplyDamage(hitCharacter, _Damage, controller,GetOwner(), UDamageType::StaticClass());
				}
			}
		
			if (!HasAuthority() && ownerPawn->IsLocallyControlled())
			{
				Server_ValidateClientHit(hitCharacter, TraceStart, Hit.Location);
			}
		}
	}
}

USceneComponent* AGun::GetFireDirectionComponent_Implementation()
{
	return RootComponent;
}

void AGun::Server_ValidateClientHit_Implementation(APawn* HitCharacter, const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation)
{
	if (!HitCharacter->Implements<UCommonUtility>())
	{
		return;
	}
	
	URollbackComponent* rollbackComponent = ICommonUtility::Execute_GetRollbackComponent(HitCharacter);
	if (!IsValid(rollbackComponent))
	{
		return;
	}
	
	APawn* owningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(owningPawn))
	{
		return;
	}
	
	AMainPlayerController* mainPlayerController = Cast<AMainPlayerController>(owningPawn->GetController());
	if (!IsValid(owningPawn))
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Applying Damage from Client"));
	FServerSideRewindResult hitRewind = rollbackComponent->ServerSideRewind(HitCharacter, TraceStart, HitLocation, mainPlayerController->GetServerTime() - mainPlayerController->_SingleTripTime);

	if (HitCharacter && hitRewind.hitConfirmed)
	{
		if (AController* controller = HitCharacter->GetController())
		{
			UGameplayStatics::ApplyDamage(HitCharacter, _Damage, controller,GetOwner(), UDamageType::StaticClass());
		}
	}
}

FVector AGun::GetShootingDirection_Implementation(float Distance)
{
	return GetActorForwardVector() * Distance + GetActorLocation();
}
