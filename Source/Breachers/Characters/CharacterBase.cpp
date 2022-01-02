#include "CharacterBase.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/MovementSystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Player");
	
	MovementSystem = CreateDefaultSubobject<UMovementSystem>(TEXT("Movement System"));
	WeaponSystem = CreateDefaultSubobject<UWeaponSystem>(TEXT("Weapon System"));
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("Health System"));
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->MaxWalkSpeed = MovementSystem->RunSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = MovementSystem->CrouchSpeed;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

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
	if(MovementSystem) MovementSystem->SetPlayerInputComponent(PlayerInputComponent);
	if(WeaponSystem) WeaponSystem->SetPlayerInputComponent(PlayerInputComponent);
}

FVector ACharacterBase::GetCameraLocation() const
{
	return CameraComponent->GetComponentLocation();
}

FVector ACharacterBase::GetCameraDirection() const
{
	return CameraComponent->GetComponentRotation().Vector();
}

USkeletalMeshComponent* ACharacterBase::GetArmsMeshFP() const
{
	return Arms_FP;
}