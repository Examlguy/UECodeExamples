#include "Possessable.h"

UInputMappingContext* IPossessable::GetControlMapping_Implementation()
{
	return nullptr;
}

bool IPossessable::CanBePossessedBy_Implementation(AMainPlayerController* PlayerController)
{
	return true;
}

TSubclassOf<UUserWidget> IPossessable::GetPossessableHUD_Implementation()
{
	return nullptr;
}