#include "CharacterBase.h"
#include "Breachers/Components/MovementSystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Player");
	MovementSystem = CreateDefaultSubobject<UMovementSystem>(TEXT("Movement System"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

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
