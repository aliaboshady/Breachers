#include "MovementSystem.h"
#include "HealthSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UMovementSystem::UMovementSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	CrouchSpeed = 200;
	WalkSpeed = 300;
	RunSpeed = 600;
	bCanTakeInput = true;
	bCanMove = true;
	bIsPlantingOrDefusing = false;
	bIsWalking = false;
	MouseSensitivityFactor = 1;
	JumpMoveFactor = 0.2;
}

void UMovementSystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(CharacterPlayer && CharacterPlayer->HealthSystem)
	{
		CharacterPlayer->HealthSystem->OnDie.AddDynamic(this, &UMovementSystem::Client_OnDie);
	}
}

void UMovementSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMovementSystem, bCanMove);
	DOREPLIFETIME(UMovementSystem, bIsPlantingOrDefusing);
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
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput || !bCanMove || bIsPlantingOrDefusing) return;
	if(!CharacterPlayer->GetCharacterMovement()->IsFalling() || MouseTurnValue > 0.1 || MouseTurnValue < -0.1)
	{
		CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorForwardVector(), Value);
	}
	else
	{
		CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorForwardVector(), Value * JumpMoveFactor);
	}
}

void UMovementSystem::MoveRight(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput || !bCanMove || bIsPlantingOrDefusing) return;
	if(CharacterPlayer->GetCharacterMovement()->IsFalling())
	{
		if(MouseTurnValue > 0.1 || MouseTurnValue < -0.1)
		{
			MoveForward(Value * MouseTurnValue);
			CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorRightVector(), Value);
		}
		else
		{
			CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorRightVector(), Value * JumpMoveFactor);
		}
	}
	else CharacterPlayer->AddMovementInput(CharacterPlayer->GetActorRightVector(), Value);
}

void UMovementSystem::LookUp(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	CharacterPlayer->AddControllerPitchInput(Value * MouseSensitivityFactor);
}

void UMovementSystem::Turn(float Value)
{
	if (Value == 0 || !CharacterPlayer || !bCanTakeInput) return;
	MouseTurnValue = Value;
	CharacterPlayer->AddControllerYawInput(Value * MouseSensitivityFactor);
}

void UMovementSystem::Jump()
{
	if (!CharacterPlayer || !bCanTakeInput || !bCanMove || bIsPlantingOrDefusing) return;
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
	if (!CharacterPlayer || !bCanTakeInput || bIsPlantingOrDefusing) return;
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

void UMovementSystem::Client_OnDie_Implementation(AController* InstigatedBy, AActor* DamageCauser)
{
	bCanTakeInput = false;
}

void UMovementSystem::SetMouseSensitivityFactor(float NewFactor)
{
	MouseSensitivityFactor = NewFactor;
}

void UMovementSystem::SetCanMove(bool bCanMovePlayer)
{
	bCanMove = bCanMovePlayer;
}

void UMovementSystem::SetIsPlantingOrDefusing(bool bPlantingOrDefusing)
{
	bIsPlantingOrDefusing = bPlantingOrDefusing;
}