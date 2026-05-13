#include "ObjectiveSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void AObjectiveSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AObjectiveSet, _SelectedObjective);
}

void AObjectiveSet::OnRep_SelectedObjective()
{
}

void AObjectiveSet::GenerateObjective()
{
	_SelectedObjective = _PotentialObjectiveItems[UKismetMathLibrary::RandomIntegerInRange(0, _PotentialObjectiveItems.Num() - 1)];
	OnRep_SelectedObjective();
}

void AObjectiveSet::ResetObjectives()
{
	for (AObjective* objective : _PotentialObjectiveItems)
	{
		objective->ResetObjective();
	}
	
	_SelectedObjective = nullptr;
}
