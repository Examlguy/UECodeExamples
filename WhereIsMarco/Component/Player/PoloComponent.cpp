#include "PoloComponent.h"
#include "NavigationSystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "UE5Coro/Coroutine.h"
#include "UE5Coro/LatentAwaiters.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/Character/Monster/MarcoCharacter.h"

UPoloComponent::UPoloComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPoloComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PlayNextQueueSound();
}

void UPoloComponent::SetAudioComponent(UAudioComponent* audio_component)
{
	_audioComponent = audio_component;
	_audioComponent->AttenuationSettings = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("UPoloComponent::SetAudioComponent"));
}

void UPoloComponent::CheckForMarco()
{
	Server_CheckForMarco();
}

void UPoloComponent::Server_CheckForMarco_Implementation()
{
	if (AWhereIsMarcoGameState* gameState = GetWorld()->GetGameState<AWhereIsMarcoGameState>())
	{
		FMarcoLocation marcoLocation = gameState->GetMarcoLocation(gameState->GetCurrentMarcoLocationClue());
		if (UKismetMathLibrary::Vector_Distance(GetOwner()->GetActorLocation(), marcoLocation._locationVector) <= _poloRequestCheckRange)
		{
			if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner()))
			{
				FHitResult hitResult;
				FCollisionQueryParams collisionParams;
				GetWorld()->LineTraceSingleByChannel(hitResult,playerCharacter->GetPawnViewLocation(), marcoLocation._locationVector, ECC_GameTraceChannel2, collisionParams);
				
				if (hitResult.bBlockingHit == false || Cast<AMarcoCharacter>(hitResult.GetActor()))
				{
					APawn* player = Cast<APawn>(GetOwner());
					if (IsValid(player))
					{
						GetWorld()->GetGameState<AWhereIsMarcoGameState>()->FoundMarcoLocation(player);
						Multicast_AddToSoundQueue(_correctSound, 1);
					}
				}
			}
			else
			{
				Multicast_AddToSoundQueue(_wrongSound, 1);
			}
		}
		else
		{
			Multicast_AddToSoundQueue(_wrongSound, 1);
		}
	}
}

void UPoloComponent::Multicast_AddToSoundQueue_Implementation(USoundBase* sound, float volume_multiplier)
{
	FAudioItem newItem;
	newItem._soundToPlay = sound;
	newItem._volumeMultiplier = volume_multiplier;
	
	_soundQueue.AddUnique(newItem);
}

void UPoloComponent::PlayNextQueueSound()
{
	if (_soundQueue.Num() > 0 && !_isBusy)
	{
		_isBusy = true;
	}
	else
	{
		return;
	}
	
	if (IsValid(_audioComponent))
	{
		FLatentActionInfo latentInfo;
		latentInfo.CallbackTarget = this;
		
		AsyncPlayNextQueueSound(latentInfo);
	}
}

FAsyncCoroutine UPoloComponent::AsyncPlayNextQueueSound(FLatentActionInfo info)
{
	_audioComponent->SetSound(_soundQueue[0]._soundToPlay);
	_audioComponent->VolumeMultiplier = _soundQueue[0]._volumeMultiplier;
	_audioComponent->Play();

	co_await UE5Coro::Latent::Seconds(_audioComponent->Sound->Duration * 0.5);
	
	_soundQueue.RemoveAt(0);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetOwner()->GetActorLocation(),1, GetOwner(), 1000);
}

void UPoloComponent::BeginPlay()
{
	_audioComponent->OnAudioFinished.AddUniqueDynamic(this, &UPoloComponent::ReadyForNextQueue);
	Super::BeginPlay();
}
