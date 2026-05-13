#pragma once

#include "CoreMinimal.h"
#include "MonsterBaseCharacter.h"
#include "Camera/CameraActor.h"
#include "UE5Coro/Coroutine.h"
#include "Components/SpotLightComponent.h"
#include "MarcoCharacter.generated.h"

UCLASS()
class WHEREISMARCO_API AMarcoCharacter : public AMonsterBaseCharacter
{
	GENERATED_BODY()

public:
	AMarcoCharacter();
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void ClearClues();
	
	UFUNCTION()
	void GenerateMarcoLocations(int locations_to_gen);

	UFUNCTION()
	void OnScreenshotCaptured(int32 width, int32 height, const TArray<FColor>& image_data);
	
	UFUNCTION()
	void OnScreenshotCompleted();

protected:
	virtual void BeginPlay() override;

	UE5Coro::TCoroutine<> GenerateScreenshotAndLocation(FForceLatentCoroutine = {});

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<ULocalLightComponent> _flashComponent;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UClass>> _objectsToSpawn;
	
private:
	UPROPERTY()
	int _numberLocationsToGenerate = 0;

	UPROPERTY()
	bool _isBusy = false;

	UPROPERTY()
	TObjectPtr<ACameraActor> _cameraActor;

	UPROPERTY(EditDefaultsOnly)
	float _widthResolution = 500;

	UPROPERTY(EditDefaultsOnly)
	float _heightResolution = 500;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> _spawnLocations;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> _cluesAndObjectsSpawned;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UClass> _clueClass;
};


