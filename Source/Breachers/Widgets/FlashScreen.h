#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FlashScreen.generated.h"

UCLASS()
class BREACHERS_API UFlashScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void GetFlashed(float FlashAmount, float FlashTime, float FlashFadeTime);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StopFlash();
};
