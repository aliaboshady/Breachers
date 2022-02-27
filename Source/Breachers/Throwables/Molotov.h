#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Molotov.generated.h"

UCLASS()
class BREACHERS_API AMolotov : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	AMolotov();
	
protected:
	virtual void OnActivate() override;
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit ) override;

	UPROPERTY(EditAnywhere)
	float ActivationSlope;
};
