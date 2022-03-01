#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Flash.generated.h"

UCLASS()
class BREACHERS_API AFlash : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	AFlash();
	virtual void OnThrow() override;

protected:
	virtual void OnActivate() override;

	UFUNCTION(Server, Reliable)
	void Server_Flash();

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float MaxFlashDistance;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float FullFlashTime;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float HalfFlashTime;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float FlashFadeTime;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "1"))
	float FullFlashAmount;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float HalfFlashAmount;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1", ClampMax = "1"))
	float FullFlashAngle;
};
