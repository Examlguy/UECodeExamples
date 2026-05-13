// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsApplication.h"
#endif

#include "WindowsFunctionLibrary.generated.h"

UCLASS()
class GHOSTGUESSR_API UWindowsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "WindowsSystemCall")
	static void RemoveTopBarOfWindow();

	UFUNCTION(BlueprintCallable, Category = "WindowsSystemCall")
	static void ShowSystemMessageBox(FText HeaderText, FText MessageText);

	UFUNCTION(BlueprintCallable, Category = "WindowsSystemCall")
	static void ShowSystemProgressBar();

private:
	static HWND GetUEWindowHandle();
};
