#include "PlantAndDefusePlayerController.h"

#include "Breachers/Components/SpectatorSystem.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Breachers/Widgets/GamePhaseBanner.h"
#include "Breachers/Widgets/Killfeed.h"

APlantAndDefusePlayerController::APlantAndDefusePlayerController()
{
	SpectatorSystem = CreateDefaultSubobject<USpectatorSystem>("SpectatorSystem");
}

void APlantAndDefusePlayerController::BeginPlay()
{
	Super::BeginPlay();
	Client_CreateBannerWidget();
	
	FTimerHandle CheckKillfeedWidgetTimerHandle;
	GetWorldTimerManager().SetTimer(CheckKillfeedWidgetTimerHandle, this, &APlantAndDefusePlayerController::Client_CheckIfKillfeedWidgetInViewport, 1, true);
}

void APlantAndDefusePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(APlantAndDefuseGameState* PDGS = Cast<APlantAndDefuseGameState>(GetWorld()->GetGameState()))
	{
		PDGS->ShowTeamsCountUI();
	}
}

void APlantAndDefusePlayerController::Client_CheckIfKillfeedWidgetInViewport_Implementation()
{
	if(KillfeedWidget && !KillfeedWidget->IsInViewport()) KillfeedWidget->AddToViewport();
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