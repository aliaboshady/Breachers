#include "CharacterAnimInstance.h"

#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"

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
	
	const FVector PlayerVelocity = OwnerCharacter->GetVelocity();
	const FRotator PlayerRotation = OwnerCharacter->GetActorRotation();
	
	Speed = PlayerVelocity.Size();
	Direction = CalculateDirection(PlayerVelocity, PlayerRotation);
	bIsJumping = OwnerCharacter->GetMovementComponent()->IsFalling();
	bIsCrouching = OwnerCharacter->GetMovementComponent()->IsCrouching();
}
