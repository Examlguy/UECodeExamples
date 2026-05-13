// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarcoComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHEREISMARCO_API UMarcoComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UMarcoComponent();
	
	UFUNCTION(BlueprintCallable)
	void Marco();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CallOutMarco();
	
	UFUNCTION(BlueprintCallable)
	void SetAudioComponent(UAudioComponent* audio_component);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintSetter=SetAudioComponent, Category = "Marco")
	TObjectPtr<UAudioComponent> _audioComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Marco")
	TObjectPtr<USoundBase> _soundToSend;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Marco")
	float _soundToSendAudioMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Marco")
	float _marcoRequestCheckRange = 1000.0f;
};
