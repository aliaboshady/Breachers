#pragma once
#include "CoreMinimal.h"
#include "BreachersPlayerController.h"
#include "Breachers/Widgets/GamePhaseBanner.h"
#include "PlantAndDefusePlayerController.generated.h"

class USpectatorSystem;
class UGamePhaseBanner;

UCLASS()
class BREACHERS_API APlantAndDefusePlayerController : public ABreachersPlayerController
{
	GENERATED_BODY()

public:
	APlantAndDefusePlayerController();
	void SwitchRoundPhaseBanner(EPhaseBanner Phase);
	void BeginSpectate();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(Client, Reliable)
	void Client_CheckIfKillfeedWidgetInViewport();

	UFUNCTION(Client, Reliable)
	void Client_CreateBannerWidget();
	
	UFUNCTION(Client, Reliable)
	void Client_CheckIfPhaseBannerInViewport();
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchRoundPhaseBanner(EPhaseBanner Phase);
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGamePhaseBanner> GamePhaseBannerWidgetClass;

	UPROPERTY(VisibleAnywhere)
	USpectatorSystem* SpectatorSystem;
	
	UPROPERTY()
	UGamePhaseBanner* GamePhaseBannerWidget;
};
