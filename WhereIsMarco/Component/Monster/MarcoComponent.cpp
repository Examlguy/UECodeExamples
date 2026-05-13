#include "MarcoComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WhereIsMarco/WhereIsMarcoGameState.h"
#include "WhereIsMarco/Component/Player/PoloComponent.h"

UMarcoComponent::UMarcoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMarcoComponent::SetAudioComponent(UAudioComponent* audio_component)
{
	_audioComponent = audio_component;
	_audioComponent->AttenuationSettings = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("UMarcoComponent::SetAudioComponent"));
}

void UMarcoComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMarcoComponent::Marco()
{
	if (IsValid(_audioComponent) && !_audioComponent->IsPlaying())
	{
		AWhereIsMarcoGameState* gameState = Cast<AWhereIsMarcoGameState>(GetWorld()->GetGameState());
		if (IsValid(gameState))
		{
			Multicast_CallOutMarco();
			
			TArray<TObjectPtr<AActor>> overlapActors;
			TArray<TEnumAsByte<EObjectTypeQuery>> overlapObjectTypes;
			TArray<AActor*> marcoRequestIgnoreActors;

			overlapObjectTypes.Add(TEnumAsByte(UEngineTypes::ConvertToObjectType(ECC_Pawn)));
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetOwner()->GetActorLocation(), _marcoRequestCheckRange, overlapObjectTypes, APlayerCharacter::StaticClass(), marcoRequestIgnoreActors,overlapActors);

			//DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), _marcoRequestCheckRange, 8, FColor::Cyan, true);

			if (overlapActors.Num() > 0)
			{
				for (AActor* overlapActor : overlapActors)
				{
					UPoloComponent* poloComponent = Cast<UPoloComponent>(overlapActor->GetComponentByClass(UPoloComponent::StaticClass()));
					if (IsValid(poloComponent))
					{
						poloComponent->Multicast_AddToSoundQueue(_soundToSend, _soundToSendAudioMultiplier);
					}
				}
			}
		}
	}
}

void UMarcoComponent::Multicast_CallOutMarco_Implementation()
{
	_audioComponent->Play();
}
