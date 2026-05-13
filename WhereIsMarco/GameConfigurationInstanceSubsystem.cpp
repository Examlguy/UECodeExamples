// Fill out your copyright notice in the Description page of Project Settings.


#include "GameConfigurationInstanceSubsystem.h"

void UGameConfigurationInstanceSubsystem::WaitForGameShadersBuilt()
{
	FStreamingManagerCollection& SMC = FStreamingManagerCollection::Get();
	SMC.BlockTillAllRequestsFinished(0.f, true);
}
