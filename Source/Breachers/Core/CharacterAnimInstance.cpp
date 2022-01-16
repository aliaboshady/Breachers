#include "CharacterAnimInstance.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(APawn* OwnerPawn = TryGetPawnOwner())
	{
		OwnerCharacter = Cast<ACharacterBase>(OwnerPawn);
	}
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!OwnerCharacter) return;
	
	Multicast_ChangePose();
	
	const FVector PlayerVelocity = OwnerCharacter->GetVelocity();
	const FRotator PlayerRotation = OwnerCharacter->GetActorRotation();
	
	Speed = PlayerVelocity.Size();
	Direction = CalculateDirection(PlayerVelocity, PlayerRotation);
	bIsJumping = OwnerCharacter->GetMovementComponent()->IsFalling();
	bIsCrouching = OwnerCharacter->GetMovementComponent()->IsCrouching();
	AimOffsetPitch = GetAimOffsetPitch();
}

float UCharacterAnimInstance::GetAimOffsetPitch() const
{
	const FRotator BaseAimRotation = OwnerCharacter->GetBaseAimRotation();
	const FRotator Rotation = OwnerCharacter->GetActorRotation();
	FRotator Delta = BaseAimRotation - Rotation;
	Delta.Normalize();
	return Delta.Pitch;
}

void UCharacterAnimInstance::Multicast_ChangePose_Implementation()
{
	if(const AWeaponBase* CurrentWeapon = OwnerCharacter->WeaponSystem->GetCurrentWeapon())
	{
		BlendSpace_ArmsTP = CurrentWeapon->WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP;
		IdlePose_ArmsTP = CurrentWeapon->WeaponInfo.WeaponAnimations.IdlePose_ArmsTP;
	}
}

void UCharacterAnimInstance::PlayFootstepSound()
{
	FHitResult OutHit;
	USoundCue* SoundCue;
	if(OwnerCharacter)
	{
		OutHit = OwnerCharacter->GetSurfaceType();
		SoundCue = GetSurfaceSound(OutHit);
		Multicast_PlayFootstepSound(SoundCue);
	}
}

void UCharacterAnimInstance::Multicast_PlayFootstepSound_Implementation(USoundCue* SoundCue)
{
	if(!SoundCue) return;
	
	FVector Location;
	FRotator Rotation;
	if(OwnerCharacter)
	{
		Location = OwnerCharacter->GetMesh()->GetComponentLocation();
		Rotation = OwnerCharacter->GetMesh()->GetComponentRotation();
	}
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundCue, Location, Rotation);
}

USoundCue* UCharacterAnimInstance::GetSurfaceSound(FHitResult OutHit)
{
	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());
	
	switch (SurfaceType)
	{
		case SURFACE_Rock:
			return FootstepSound_Rock;
		case SURFACE_Tile:
			return FootstepSound_Tile;
		default: return nullptr;
	}
}