#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Smoke.generated.h"

UCLASS()
class BREACHERS_API ASmoke : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	ASmoke();
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnThrow() override;
	void SetToThrown();

	UFUNCTION(Server, Reliable)
	void Server_CheckSpeed();

	UPROPERTY(EditAnywhere)
	float ActivateAfterTime;

	UPROPERTY(EditAnywhere)
	float StopTimeBeforeActivation;
	
	UPROPERTY(EditAnywhere)
	float ActivationSpeed;

	float StoppedForTime;
};
