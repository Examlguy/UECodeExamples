#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActivatableInterface.generated.h"

UINTERFACE()
class UActivatableInterface : public UInterface
{
	GENERATED_BODY()
};

class GHOSTGUESSR_API IActivatableInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ToggleActivate(bool ActiveState, AController* Activator);
	virtual void ToggleActivate_Implementation(bool ActiveState, AController* Activator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsActivated();
	virtual bool GetIsActivated_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetPromptText();
	virtual FText GetPromptText_Implementation();
};
