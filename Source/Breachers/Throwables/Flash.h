#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Flash.generated.h"

UCLASS()
class BREACHERS_API AFlash : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	virtual void OnThrow() override;

	virtual void OnActivate() override;

	UFUNCTION(Server, Reliable)
	void Server_Flash();
};
