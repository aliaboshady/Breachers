#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Molotov.generated.h"

class AFire;

UCLASS()
class BREACHERS_API AMolotov : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	AMolotov();
	
protected:
	virtual void OnActivate() override;
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit ) override;

	UFUNCTION(Server, Reliable)
	void Server_SpawnFire();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AFire> FireClass;
	
	UPROPERTY(EditAnywhere)
	float ActivationSlope;
};
