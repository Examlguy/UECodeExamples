// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UE5Coro/AsyncCoroutine.h"
#include "EulerTransform.h"
#include "PoloComponent.generated.h"

USTRUCT(BlueprintType)
struct FAudioItem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<USoundBase> _soundToPlay = nullptr;
	
	UPROPERTY()
	float _volumeMultiplier = 1.f;

	bool operator == (const FAudioItem& audio_item) const
	{
		return _soundToPlay == audio_item._soundToPlay && _volumeMultiplier == audio_item._volumeMultiplier;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHEREISMARCO_API UPoloComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPoloComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);
	
	UFUNCTION(BlueprintCallable)
	void SetAudioComponent(UAudioComponent* audio_component);

	UFUNCTION(BlueprintCallable)
	void CheckForMarco();

	UFUNCTION(Server, Reliable)
	void Server_CheckForMarco();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_AddToSoundQueue(USoundBase* sound, float volume_multiplier);

	UFUNCTION(BlueprintCallable)
	void PlayNextQueueSound();
	
	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = info))
	FAsyncCoroutine AsyncPlayNextQueueSound(FLatentActionInfo info);
	
protected:
	UFUNCTION()
	void ReadyForNextQueue() { _isBusy = false; }
	
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintSetter=SetAudioComponent, Category = "MarcoPolo")
	TObjectPtr<UAudioComponent> _audioComponent;
	
	UPROPERTY(BlueprintReadWrite, Category = "MarcoPolo")
	TArray<FAudioItem> _soundQueue;

	UPROPERTY(BlueprintReadWrite, Category = "MarcoPolo")
	bool _isBusy = false;

	UPROPERTY(EditAnywhere, Category = "MarcoPolo")
	float _poloRequestCheckRange = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MarcoPolo")
	TObjectPtr<USoundBase> _correctSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MarcoPolo")
	TObjectPtr<USoundBase> _wrongSound;
};
