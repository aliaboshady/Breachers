#include "PlantAndDefusePlayerController.h"
#include "Breachers/Widgets/GamePhaseBanner.h"

void APlantAndDefusePlayerController::BeginPlay()
{
	Super::BeginPlay();
	Client_CreateBannerWidget();
}

void APlantAndDefusePlayerController::Client_CreateBannerWidget_Implementation()
{
	if(GamePhaseBannerWidgetClass)
	{
		GamePhaseBannerWidget = CreateWidget<UGamePhaseBanner>(this, GamePhaseBannerWidgetClass);
		if(GamePhaseBannerWidget) GamePhaseBannerWidget->AddToViewport();
	}

	FTimerHandle CheckPhaseBannerTimerHandle;
	GetWorldTimerManager().SetTimer(CheckPhaseBannerTimerHandle, this, &APlantAndDefusePlayerController::Client_CheckIfPhaseBannerInViewport, 1, true);
}

void APlantAndDefusePlayerController::Client_CheckIfPhaseBannerInViewport_Implementation()
{
	if(GamePhaseBannerWidget && !GamePhaseBannerWidget->IsInViewport()) GamePhaseBannerWidget->AddToViewport();
}

void APlantAndDefusePlayerController::SwitchRoundPhaseBanner(EPhaseBanner Phase)
{
	Client_SwitchRoundPhaseBanner(Phase);
}

void APlantAndDefusePlayerController::Client_SwitchRoundPhaseBanner_Implementation(EPhaseBanner Phase)
{
	if(GamePhaseBannerWidget) GamePhaseBannerWidget->SwitchRoundPhaseBanner(Phase);
}