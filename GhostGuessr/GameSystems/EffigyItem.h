// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EffigyItem.generated.h"

UCLASS()
class GHOSTGUESSR_API AEffigyItem : public AActor
{
	GENERATED_BODY()

public:
	AEffigyItem();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetFindable(bool Findable);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MakeEditWidget))
	FVector _ScreenshotFromLocation;
};
