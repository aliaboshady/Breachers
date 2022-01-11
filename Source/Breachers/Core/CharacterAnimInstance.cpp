#include "CharacterAnimInstance.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "GameFramework/PawnMovementComponent.h"

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(APawn* OwnerPawn = TryGetPawnOwner())
	{
		OwnerCharacter = Cast<ACharacterBase>(OwnerPawn);
	}
	bDidBeginPlay = false;
	FTimerHandle BeginTimer;
	GetWorld()->GetTimerManager().SetTimer(BeginTimer, this, &UCharacterAnimInstance::BeginAfterTime, 1, false, 0.1);
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

void UCharacterAnimInstance::BeginAfterTime()
{
	bDidBeginPlay = true;
}
