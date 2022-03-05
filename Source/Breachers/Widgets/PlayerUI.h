#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

UCLASS()
class BREACHERS_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetPlantDefuseProgress(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void StopPlantDefuseProgress();
};
