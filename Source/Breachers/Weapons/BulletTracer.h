#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletTracer.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BREACHERS_API ABulletTracer : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletTracer();
	float GetBulletSpeed();

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* TracerParticle;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;
};
