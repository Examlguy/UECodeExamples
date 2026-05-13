#include "CovertCounterOpsWorldSettings.h"

#include "Engine/StaticMeshActor.h"

TObjectPtr<ACovertCounterOpsWorldSettings> ACovertCounterOpsWorldSettings::_WorldSettings;

void ACovertCounterOpsWorldSettings::BeginPlay()
{
	Super::BeginPlay();
	
	_WorldSettings = this;
	
	TArray<ULevelStreaming*> StreamingLevels = GetWorld()->GetStreamingLevels();
	_StreamedLevels.SetNumUninitialized(StreamingLevels.Num());
	
	for (ULevelStreaming* level : StreamingLevels)
	{
		for (int i = 0; i < _SublevelByFloors.Num(); ++i)
		{
			if (level->GetWorldAsset()->GetName() == _SublevelByFloors[i])
			{
				_StreamedLevels[i] = level;
			}
		}
	}
}

void ACovertCounterOpsWorldSettings::SetFloorVisibility(int FloorNumber, bool IsHidden)
{
	ULevel* level = _StreamedLevels[FloorNumber]->GetLoadedLevel();
	if (!IsValid(level))
	{
		return;
	}
	
	for (auto mesh : level->Actors)
	{
		if (AStaticMeshActor* staticMesh = Cast<AStaticMeshActor>(mesh))
		{
			staticMesh->SetActorHiddenInGame(IsHidden);
		}
	}
}

void ACovertCounterOpsWorldSettings::ShowAllFloors()
{
	for (int i = 0; i < _StreamedLevels.Num(); ++i)
	{
		if (_StreamedLevels[i]->IsLevelLoaded())
		{
			SetFloorVisibility(i, false);
		}
	}
}

void ACovertCounterOpsWorldSettings::ViewPreviousFloor()
{
	if (_CurrentViewedFloor - 1 >= 0)
	{
		_CurrentViewedFloor--;
		SetFloorVisibility(_CurrentViewedFloor, false);
	}
}

void ACovertCounterOpsWorldSettings::ViewNextFloor()
{
	if (_CurrentViewedFloor + 1 < _StreamedLevels.Num())
	{
		SetFloorVisibility(_CurrentViewedFloor, true);
		_CurrentViewedFloor++;
	}
}