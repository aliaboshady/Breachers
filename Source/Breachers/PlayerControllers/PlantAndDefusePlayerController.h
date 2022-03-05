#pragma once
#include "CoreMinimal.h"
#include "BreachersPlayerController.h"
#include "Breachers/Widgets/GamePhaseBanner.h"
#include "PlantAndDefusePlayerController.generated.h"

class UFlashComponent;
class UGamePhaseBanner;

UCLASS()
class BREACHERS_API APlantAndDefusePlayerController : public ABreachersPlayerController
{
	GENERATED_BODY()

public:
	APlantAndDefusePlayerController();
	void SwitchRoundPhaseBanner(EPhaseBanner Phase);
	void BeginSpectate();
	void SetPlantDefuseProgress(float Duration);
	void StopPlantDefuseProgress();

	UPROPERTY(VisibleAnywhere)
	UFlashComponent* FlashComponent;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void SpectateModeStart();

	UFUNCTION(Client, Reliable)
	void Client_CheckIfKillfeedWidgetInViewport();

	UFUNCTION(Client, Reliable)
	void Client_CreateBannerWidget();
	
	UFUNCTION(Client, Reliable)
	void Client_CheckIfPhaseBannerInViewport();
	
	UFUNCTION(Client, Reliable)
	void Client_SwitchRoundPhaseBanner(EPhaseBanner Phase);

	UFUNCTION(Client, Reliable)
	void Client_SetPlantDefuseProgress(float Duration);

	UFUNCTION(Client, Reliable)
	void Client_StopPlantDefuseProgress();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGamePhaseBanner> GamePhaseBannerWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASpectatorPawn> SpectatorPawnClass;

	UPROPERTY(EditAnywhere)
	float StartSpectateAfter;
	
	UPROPERTY()
	UGamePhaseBanner* GamePhaseBannerWidget;

	FTimerHandle SpectatorTimerHandle;
};
