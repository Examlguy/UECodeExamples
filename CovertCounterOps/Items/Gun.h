#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Gun.generated.h"

USTRUCT(BlueprintType)
struct FGunState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int ShotCount = 0;
	
	UPROPERTY()
	float Time = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGunFired, FVector, FireDirection);

UCLASS()
class COVERTCOUNTEROPS_API AGun : public AItem
{
	GENERATED_BODY()

public:
	AGun();

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable)
	void Fire();
	
	UFUNCTION()
	void FireBullet();
	
	UFUNCTION(Server, Reliable,BlueprintCallable)
	void Server_ValidateClientHit(APawn* HitCharacter, const FVector_NetQuantize TraceStart, const FVector_NetQuantize HitLocation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USceneComponent* GetFireDirectionComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	FVector GetShootingDirection(float Distance = 10000);

	UPROPERTY(BlueprintAssignable)
	FOnGunFired OnGunFiredDelegate;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USoundBase> _ShootSound;
	
private:
	UPROPERTY()
	float _LastFireTime = 0;
	float _TimeBetweenShots;
	FRandomStream _RandomStream;
	
	UPROPERTY(EditDefaultsOnly)
	float _Damage = 35;
	
	UPROPERTY(EditDefaultsOnly)
	float _RoundsPerMinute = 500;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin=1))
	int _MaxSpreadDistance = 1;
	
	UPROPERTY(EditDefaultsOnly)
	float _Distance = 500;
};
