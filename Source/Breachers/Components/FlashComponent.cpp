#include "FlashComponent.h"
#include "Blueprint/UserWidget.h"
#include "Breachers/PlayerControllers/PlantAndDefusePlayerController.h"
#include "Breachers/Widgets/FlashScreen.h"

UFlashComponent::UFlashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlashComponent::Client_CreateFlashScreenWidget_Implementation()
{
	APlantAndDefusePlayerController* PDPC = Cast<APlantAndDefusePlayerController>(GetOwner());
	if(PDPC && FlashScreenClass) FlashScreenWidget = CreateWidget<UFlashScreen>(PDPC, FlashScreenClass);
	if(FlashScreenWidget) FlashScreenWidget->AddToViewport();
}

void UFlashComponent::GetFlashed(float FlashAmount, float FlashTime, float FlashFadeTime)
{
	Client_GetFlashed(FlashAmount, FlashTime, FlashFadeTime);
}

void UFlashComponent::Client_GetFlashed_Implementation(float FlashAmount, float FlashTime, float FlashFadeTime)
{
	if(!FlashScreenWidget) Client_CreateFlashScreenWidget();
	if(FlashScreenWidget) FlashScreenWidget->GetFlashed(FlashAmount, FlashTime, FlashFadeTime);
}

void UFlashComponent::StopFlash()
{
	Client_StopFlash();
}

void UFlashComponent::Client_StopFlash_Implementation()
{
	if(!FlashScreenWidget) Client_CreateFlashScreenWidget();
	if(FlashScreenWidget) FlashScreenWidget->StopFlash();
}
