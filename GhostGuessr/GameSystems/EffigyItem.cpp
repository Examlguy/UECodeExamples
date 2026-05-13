#include "EffigyItem.h"

AEffigyItem::AEffigyItem()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEffigyItem::BeginPlay()
{
	Super::BeginPlay();
}

void AEffigyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEffigyItem::SetFindable_Implementation(bool Findable)
{
	// no op
}

