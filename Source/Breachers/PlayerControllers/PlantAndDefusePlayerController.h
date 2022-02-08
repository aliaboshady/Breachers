#pragma once
#include "CoreMinimal.h"
#include "BreachersPlayerController.h"
#include "Breachers/Widgets/GamePhaseBanner.h"
#include "PlantAndDefusePlayerController.generated.h"

class UGamePhaseBanner;

UCLASS()
class BREACHERS_API APlantAndDefusePlayerController : public ABreachersPlayerController
{
	GENERATED_BODY()

public:
	void SwitchRoundPhaseBanner(EPhaseBanner Phase);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_CheckIfPhaseBannerInViewport();
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchRoundPhaseBanner(EPhaseBanner Phase);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGamePhaseBanner> GamePhaseBannerWidgetClass;
	
	UPROPERTY()
	UGamePhaseBanner* GamePhaseBannerWidget;
};
