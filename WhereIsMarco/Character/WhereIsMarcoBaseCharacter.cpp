// Fill out your copyright notice in the Description page of Project Settings.


#include "WhereIsMarcoBaseCharacter.h"

#include "Net/UnrealNetwork.h"


// Sets default values
AWhereIsMarcoBaseCharacter::AWhereIsMarcoBaseCharacter() : _sightTeamComponent(CreateDefaultSubobject<UAdvancedSightTeamComponent>(TEXT("SightTeam")))
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWhereIsMarcoBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWhereIsMarcoBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AWhereIsMarcoBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

