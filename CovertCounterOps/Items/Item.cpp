#include "Item.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "CovertCounterOps/Game/A1VXGameState.h"

AItem::AItem()
{
}

void AItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, _LatestItemData);
	DOREPLIFETIME(AItem, _IsUsing);
}

void AItem::SetIsUsing(bool NewIsUsing)
{
	if (NewIsUsing != _IsUsing)
	{
		_IsUsing = NewIsUsing;
		if (!HasAuthority())
		{
			Server_ToggleUsing(NewIsUsing);
		}
		
		if (_IsUsing)
		{
			OnStartUsing();
		}
		else
		{
			OnStopUsing();
		}
	}
}

void AItem::Pickup(APawn* GrabbingPawn, USceneComponent* GrabbingPawnComponent, FName BoneName)
{
	FItemData newData;
	newData.IsHolding = true;
	newData.GrabbingPawn = GrabbingPawn;
	newData.GrabbingPawnComponent = GrabbingPawnComponent;
	newData.HoldingBoneName = BoneName;
	newData.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	_LocalItemData = newData;

	OnPickedUp(GrabbingPawn, GrabbingPawnComponent);
	AttachToComponent(GrabbingPawnComponent,FAttachmentTransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), BoneName);
	
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		SetInstigator(GrabbingPawn);
		SetOwner(GrabbingPawn);
		_LatestItemData = newData;
	}
}

void AItem::Server_ToggleUsing_Implementation(bool IsUsing)
{
	_IsUsing = IsUsing;
}

void AItem::Drop()
{
	FItemData newData;
	newData.IsHolding = false;
	newData.GrabbingPawn = nullptr;
	newData.GrabbingPawnComponent = nullptr;
	newData.HoldingBoneName = "";
	newData.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	_LocalItemData = newData;

	OnDropped();
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
	
	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		SetInstigator(nullptr);
		SetOwner(nullptr);
		_LatestItemData = newData;
	}
}

void AItem::OnRep_ItemData()
{
	if (_LatestItemData.Time > _LocalItemData.Time)
	{
		if (_LatestItemData.IsHolding)
		{
			Pickup(_LatestItemData.GrabbingPawn, _LatestItemData.GrabbingPawnComponent, _LatestItemData.HoldingBoneName);
		}
		else
		{
			Drop();
		}
	}
}

void AItem::OnRep_IsUsing(bool LastIsUsing)
{
	if (APawn* ownerPawn = Cast<APawn>(GetOwner()))
	{
		if (!HasAuthority() && ownerPawn->IsLocallyControlled())
		{
			if (LastIsUsing != _IsUsing)
			{
				_IsUsing = LastIsUsing;
			}
		}
	}
}
