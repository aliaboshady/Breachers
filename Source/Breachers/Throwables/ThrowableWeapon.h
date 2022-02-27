#pragma once
#include "CoreMinimal.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "ThrowableWeapon.generated.h"

UCLASS()
class BREACHERS_API AThrowableWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AThrowableWeapon();
	virtual void OnPrimaryFire() override;
	virtual void OnSecondaryFire() override;
	virtual void OnThrow();
	float GetCurrentThrowForce();
	void SetThrowerCharacter(ACharacterBase* NewThrowerCharacter);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipPreviousWeaponAfterThrow();
	void OnFire(float ThrowForce);
	virtual void OnActivate();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit );

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetThrowForce(float NewThrowForce);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayThrowAnimation();
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_ActivationEffects();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayBounceSound();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCanPlayBounceSound(bool bCanPlaySound);

	UPROPERTY(Replicated)
	ACharacterBase* ThrowerCharacter;

	UPROPERTY(EditAnywhere)
	float PrimaryThrowForce;

	UPROPERTY(EditAnywhere)
	float SecondaryThrowForce;

	UPROPERTY(EditAnywhere)
	float TimeToActivate;

	UPROPERTY(EditAnywhere)
	float ActivationDuration;

	UPROPERTY(EditAnywhere)
	float ImpactSoundPower;

	UPROPERTY(Replicated)
	float CurrentThrowForce;

	bool bCanPlayBounceSound;
};