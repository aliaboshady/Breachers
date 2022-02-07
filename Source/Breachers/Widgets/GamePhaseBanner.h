#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePhaseBanner.generated.h"

UENUM(BlueprintType)
enum EPhaseBanner
{
	BuyPhaseBanner,
	MainPhaseBanner,
	AttackersWinBanner,
	DefendersWinBanner
};

UCLASS()
class BREACHERS_API UGamePhaseBanner : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchRoundPhaseBanner(EPhaseBanner Phase);
};
