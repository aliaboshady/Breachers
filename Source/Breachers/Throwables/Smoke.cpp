#include "Smoke.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"

ASmoke::ASmoke()
{
	ActivateAfterTime = 2;
	StopTimeBeforeActivation = 1;
	ActivationSpeed = 150;
}

void ASmoke::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Server_CheckSpeed();
	StoppedForTime += DeltaSeconds;
}

void ASmoke::OnThrow()
{
	Super::OnThrow();
	FTimerHandle ThrownTimerHandle;
	GetWorldTimerManager().SetTimer(ThrownTimerHandle, this, &ASmoke::SetToThrown, 1, false, 0.5);
}

void ASmoke::SetToThrown()
{
	SetActorTickEnabled(true);
}

void ASmoke::Multicast_ActivationEffects_Implementation()
{
	UParticleSystem* ExplosionParticles = WeaponInfo.WeaponEffects.MuzzleFlashEffect;
	if(ExplosionParticles)
	{
		UParticleSystemComponent* SmokeParticles = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, GetActorTransform());
		FParticleSysParam LifeTimeParameter;
		LifeTimeParameter.Name = "SmokeDuration";
		LifeTimeParameter.ParamType = PSPT_Scalar;
		LifeTimeParameter.Scalar = ActivationDuration;
		SmokeParticles->InstanceParameters.Add(LifeTimeParameter);
	}

	USoundCue* ExplosionSound = WeaponInfo.WeaponEffects.MuzzleFireSound;
	if(ExplosionSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
}

void ASmoke::Server_CheckSpeed_Implementation()
{
	if(GetVelocity().Size() <= ActivationSpeed)
	{
		if(StoppedForTime >= StopTimeBeforeActivation)
		{
			FTimerHandle ActivateTimerHandle;
			GetWorldTimerManager().SetTimer(ActivateTimerHandle, this, &ASmoke::OnActivate, 1, false, ActivateAfterTime);
			SetActorTickEnabled(false);
		}
	}
	else StoppedForTime = 0;
}
