#include "CharacterBase.h"
#include "Breachers/Components/MovementSystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Player");
	CrouchSpeed = 200;
	WalkSpeed = 300;
	RunSpeed = 600;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	MovementSystem = CreateDefaultSubobject<UMovementSystem>(TEXT("Movement System"));
	WeaponSystem = CreateDefaultSubobject<UWeaponSystem>(TEXT("Weapon System"));

	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bCastHiddenShadow = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0, 0, 70));
	CameraComponent->bUsePawnControlRotation = true;

	Arms_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms FP"));
	Arms_FP->SetupAttachment(CameraComponent);
	Arms_FP->SetOnlyOwnerSee(true);
	Arms_FP->SetRelativeLocation(FVector(-20, 0, -170));
	Arms_FP->SetRelativeRotation(FRotator(0, -90, 0));
	Arms_FP->CastShadow = false;
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ACharacterBase, WeaponSystem);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ACharacterBase::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterBase::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterBase::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACharacterBase::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACharacterBase::StopCrouch);
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ACharacterBase::Server_StartWalk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ACharacterBase::Server_StopWalk);
	
	PlayerInputComponent->BindAction("EquipPrimary", IE_Pressed, this, &ACharacterBase::EquipPrimary);
	PlayerInputComponent->BindAction("EquipSecondary", IE_Pressed, this, &ACharacterBase::EquipSecondary);
	PlayerInputComponent->BindAction("EquipMelee", IE_Pressed, this, &ACharacterBase::EquipMelee);
}

void ACharacterBase::MoveForward(float Value)
{
	if(!MovementSystem) return;
	MovementSystem->MoveForward(Value, this);
}

void ACharacterBase::MoveRight(float Value)
{
	if(!MovementSystem) return;
	MovementSystem->MoveRight(Value, this);
}

void ACharacterBase::StartCrouch()
{
	Crouch();
}

void ACharacterBase::StopCrouch()
{
	UnCrouch();
}

void ACharacterBase::Client_StartWalk_Implementation()
{
	StartWalk();
}

void ACharacterBase::Server_StartWalk_Implementation()
{
	StartWalk();
	Client_StartWalk();
}

void ACharacterBase::StartWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}


void ACharacterBase::Client_StopWalk_Implementation()
{
	StopWalk();
}

void ACharacterBase::Server_StopWalk_Implementation()
{
	StopWalk();
	Client_StopWalk();
}

void ACharacterBase::StopWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

USkeletalMeshComponent* ACharacterBase::GetArmsMeshFP()
{
	return Arms_FP;
}

void ACharacterBase::EquipWeapon(AWeaponBase* Weapon)
{
	WeaponSystem->EquipWeapon(Weapon);
}


bool ACharacterBase::CanTakeWeapon(AWeaponBase* Weapon)
{
	return WeaponSystem->CanTakeWeapon(Weapon);
}

void ACharacterBase::TakeWeapon(AWeaponBase* Weapon)
{
	Server_TakeWeapon(Weapon);
}

void ACharacterBase::Server_TakeWeapon_Implementation(AWeaponBase* Weapon)
{
	WeaponSystem->TakeWeapon(Weapon);
}

void ACharacterBase::EquipPrimary()
{
	WeaponSystem->EquipPrimary();
}

void ACharacterBase::EquipSecondary()
{
	WeaponSystem->EquipSecondary();
}

void ACharacterBase::EquipMelee()
{
	WeaponSystem->EquipMelee();
}