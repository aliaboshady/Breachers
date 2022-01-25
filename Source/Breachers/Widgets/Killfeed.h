#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Killfeed.generated.h"

UCLASS()
class BREACHERS_API UKillfeed : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void AddKillfeedRow(FName KillerName, UTexture2D* WeaponIcon, FName KilledName);
};
