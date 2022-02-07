#pragma once
#include "CoreMinimal.h"
#include "BreachersPlayerController.h"
#include "PlantAndDefusePlayerController.generated.h"

class UGamePhaseBanner;

UCLASS()
class BREACHERS_API APlantAndDefusePlayerController : public ABreachersPlayerController
{
	GENERATED_BODY()

protected:
	//virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGamePhaseBanner> UGamePhaseBannerWidgetClass;
	
	UPROPERTY()
	UGamePhaseBanner* UGamePhaseBannerWidget;
};
