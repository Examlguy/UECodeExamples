#pragma once

#include "CoreMinimal.h"
#include "AdvancedSightTeamComponent.h"
#include "GenericTeamAgentInterface.h"
#include "GMCPawn.h"
#include "WhereIsMarcoBaseCharacter.generated.h"

UCLASS()
class WHEREISMARCO_API AWhereIsMarcoBaseCharacter : public AGMC_Pawn, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AWhereIsMarcoBaseCharacter();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetMovementEnabled(bool enabled) { _movementEnabled = enabled; }

	UFUNCTION(BlueprintCallable)
	virtual bool GetMovementEnabled() const { return _movementEnabled; }
	
	UFUNCTION(BlueprintCallable)
	virtual void SetCharacterEnabled(bool enabled) { _characterEnabled = enabled; }

	UFUNCTION(BlueprintCallable)
	virtual bool GetCharacterEnabled() const { return _characterEnabled; }

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(_sightTeamComponent->TeamID); }

	UFUNCTION(BlueprintCallable)
	FVector GetEyeHeightOffset() const { return FVector(0,0, BaseEyeHeight); }
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintSetter=SetMovementEnabled, BlueprintGetter=GetMovementEnabled, Category = "Character")
	bool _movementEnabled = true;
	
	UPROPERTY(EditAnywhere, BlueprintSetter=SetCharacterEnabled, Blueprintgetter=GetCharacterEnabled, Category = "Character");
	bool _characterEnabled = true;
	
	UPROPERTY(EditAnywhere)
	const TObjectPtr<UAdvancedSightTeamComponent> _sightTeamComponent;
};
