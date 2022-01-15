#include "MovementSystem.h"
#include "HealthSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UMovementSystem::UMovementSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	CrouchSpeed = 200;
	WalkSpeed = 300;
	RunSpeed = 600;
	bCanTakeInput = true;
	bIsWalking = false;
}

void UMovementSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(CharacterPlayer && CharacterPlayer->HealthSystem)
	{
		CharacterPlayer->HealthSystem->OnDie.AddDynamic(this, &UMovementSystem::Client_OnDie);
	}
}

void UMovementSystem::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent && CharacterPlayer)
	{
		PlayerInputComponent->BindAxis(INPUT_MoveForward, this, &UMovementSystem::MoveForward);
		PlayerInputComponent->BindAxis(INPUT_MoveRight, this, &UMovementSystem::MoveRight);
		
		PlayerInputComponent->BindAxis(INPUT_LookUp, this, &UMovementSystem::LookUp);
		PlayerInputComponent->BindAxis(INPUT_Turn, this, &UMovementSystem::Turn);
		
		PlayerInputComponent->BindAction(INPUT_Jump, IE_Pressed, this, &UMovementSystem::Jump);
		PlayerInputComponent->BindAction(INPUT_Jump, IE_Released, this, &UMovementSystem::StopJumping);
		
		PlayerInputComponent->BindAction(INPUT_Crouch, IE_Pressed, this, &UMovementSystem::StartCrouch);
		PlayerInputComponent->BindAction(INPUT_Crouch, IE_Released, this, &UMovementSystem::StopCrouch);
		
		PlayerInputComponent->BindAction(INPUT_Walk, IE_Pressed, this, &UMovementSystem::Server_StartWalk);
		PlayerInputComponent->BindAction(INPUT_Walk, IE_Released, this, &UMovementSystem::Server_StopWalk);
	}
}


void UMovementSystem::Server_SetSpeedsOfWeapon_Implementation(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed)
{
	PR_Client_SetSpeedsOfWeapon(WeaponRunSpeed, WeaponWalkSpeed, WeaponCrouchSpeed);
	Client_SetSpeedsOfWeapon(WeaponRunSpeed, WeaponWalkSpeed, WeaponCrouchSpeed);
}

void UMovementSystem::Client_SetSpeedsOfWeapon_Implementation(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed)
{
	PR_Client_SetSpeedsOfWeapon(WeaponRunSpeed, WeaponWalkSpeed, WeaponCrouchSpeed);
}

void UMovementSystem::PR_Client_SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed)
{
	CrouchSpeed = WeaponCrouchSpeed;
	WalkSpeed = WeaponWalkSpeed;
	RunSpeed = WeaponRunSpeed;
	if(bIsWalking) CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	else CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	CharacterPlayer->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

void UMovementSystem::SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed)
{
	Server_SetSpeedsOfWeapon(WeaponRunSpeed, WeaponWalkSpeed, WeaponCrouchSpeed);
}

void UMovementSystem::MoveForward(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorForwardVector(), Value);
}

void UMovementSystem::MoveRight(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorRightVector(), Value);
}

void UMovementSystem::LookUp(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->AddControllerPitchInput(Value);
}

void UMovementSystem::Turn(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->AddControllerYawInput(Value);
}

void UMovementSystem::Jump()
{
	if (!CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->Jump();
}

void UMovementSystem::StopJumping()
{
	if (!CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->StopJumping();
}

void UMovementSystem::StartCrouch()
{
	if (!CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->Crouch();
}

void UMovementSystem::StopCrouch()
{
	if (!CharacterPlayer || !bCanTakeInput) return;
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
	if (!CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bIsWalking = true;
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
	if (!CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	bIsWalking = false;
}

void UMovementSystem::Client_OnDie_Implementation()
{
	bCanTakeInput = false;
}
