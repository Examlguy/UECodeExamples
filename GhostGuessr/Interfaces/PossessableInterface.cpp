#include "PossessableInterface.h"

UInputMappingContext* IPossessableInterface::GetControlMapping_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("GetControlMapping_Implementation IS NOT IMPLEMENTED! YOU SHOULD!"));
	return nullptr;
}

bool IPossessableInterface::CanBePossessedBy_Implementation(AController* Controller)
{
	return true;
}

TSubclassOf<UUserWidget> IPossessableInterface::GetPossessableHUD_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("GetPossessableHUD_Implementation IS NOT IMPLEMENTED! YOU SHOULD!"));
	return nullptr;
}