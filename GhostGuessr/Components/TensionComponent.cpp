#include "TensionComponent.h"

UTensionComponent::UTensionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTensionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTensionComponent::ApplyTension_Implementation(ETensionType TensionType, float Multiplier)
{
	// no op
}

void UTensionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

