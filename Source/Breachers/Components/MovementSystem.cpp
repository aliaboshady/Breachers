#include "MovementSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementSystem::UMovementSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	CrouchSpeed = 200;
	WalkSpeed = 300;
	RunSpeed = 600;
}

void UMovementSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
}

void UMovementSystem::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent && CharacterPlayer)
	{
		PlayerInputComponent->BindAxis("MoveForward", this, &UMovementSystem::MoveForward);
		PlayerInputComponent->BindAxis("MoveRight", this, &UMovementSystem::MoveRight);
		
		PlayerInputComponent->BindAxis("LookUp", this, &UMovementSystem::LookUp);
		PlayerInputComponent->BindAxis("Turn", this, &UMovementSystem::Turn);
		
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &UMovementSystem::Jump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &UMovementSystem::StopJumping);
		
		PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &UMovementSystem::StartCrouch);
		PlayerInputComponent->BindAction("Crouch", IE_Released, this, &UMovementSystem::StopCrouch);
		
		PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &UMovementSystem::Server_StartWalk);
		PlayerInputComponent->BindAction("Walk", IE_Released, this, &UMovementSystem::Server_StopWalk);
	}
}

void UMovementSystem::MoveForward(float Value)
{
	if (Value == 0 || !CharacterPlayer) return;
	CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorForwardVector(), Value);
}

void UMovementSystem::MoveRight(float Value)
{
	if (Value == 0 || !CharacterPlayer) return;
	CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorRightVector(), Value);
}

void UMovementSystem::LookUp(float Value)
{
	if (Value == 0 || !CharacterPlayer) return;
	CharacterPlayer->AddControllerPitchInput(Value);
}

void UMovementSystem::Turn(float Value)
{
	if (Value == 0 || !CharacterPlayer) return;
	CharacterPlayer->AddControllerYawInput(Value);
}

void UMovementSystem::Jump()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->Jump();
}

void UMovementSystem::StopJumping()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->StopJumping();
}

void UMovementSystem::StartCrouch()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->Crouch();
}

void UMovementSystem::StopCrouch()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->UnCrouch();
}

void UMovementSystem::Client_StartWalk_Implementation()
{
	StartWalk();
}

void UMovementSystem::Server_StartWalk_Implementation()
{
	StartWalk();
	Client_StartWalk();
}

void UMovementSystem::StartWalk()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void UMovementSystem::Client_StopWalk_Implementation()
{
	StopWalk();
}

void UMovementSystem::Server_StopWalk_Implementation()
{
	StopWalk();
	Client_StopWalk();
}

void UMovementSystem::StopWalk()
{
	if (!CharacterPlayer) return;
	CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}