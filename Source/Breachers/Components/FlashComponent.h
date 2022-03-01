#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlashComponent.generated.h"

class UFlashScreen;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFlashComponent();
	void GetFlashed(float FlashAmount, float FlashTime, float FlashFadeTime, bool bIsFullFlash = true);
	void StopFlash();

protected:
	UFUNCTION(Client, Reliable)
	void Client_GetFlashed(float FlashAmount, float FlashTime, float FlashFadeTime, bool bIsFullFlash);
	
	UFUNCTION(Client, Reliable)
	void Client_StopFlash();
	
	UFUNCTION(Client, Reliable)
	void Client_CreateFlashScreenWidget();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFlashScreen> FlashScreenClass;

	UPROPERTY()
	UFlashScreen* FlashScreenWidget;
};
