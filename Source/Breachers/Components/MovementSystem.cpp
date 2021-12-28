#include "MovementSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementSystem::UMovementSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMovementSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
}

void UMovementSystem::MoveForward(float Value, APawn* Pawn)
{
	if (Value == 0 || !Pawn) return;
	Pawn->AddMovementInput(Pawn->GetActorForwardVector(), Value);
}

void UMovementSystem::MoveRight(float Value, APawn* Pawn)
{
	if (Value == 0 || !Pawn) return;
	Pawn->AddMovementInput(Pawn->GetActorRightVector(), Value);
}

// void UMovementSystem::SetSpeedSlow(UCharacterMovementComponent* PlayerMovementComponent)
// {
// 	PlayerMovementComponent->MaxWalkSpeed()
// }
//
// void UMovementSystem::SetSpeedFast(UCharacterMovementComponent* PlayerMovementComponent)
// {
// }
