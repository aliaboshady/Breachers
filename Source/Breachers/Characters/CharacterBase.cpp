#include "CharacterBase.h"
#include "Breachers/Components/BuyMenu.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/MovementSystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameModes/BreachersGameModeBase.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add(TAG_Player);
	Team = Attacker;
	
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
	GetMesh()->SetCollisionProfileName(COLLISION_CharacterPlayerTP, true);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(0, 0, 70));
	CameraComponent->bUsePawnControlRotation = true;

	DeathSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Death Spring Arm"));
	DeathSpringArm->SetupAttachment(RootComponent);
	DeathSpringArm->SetRelativeLocation(FVector(0, 0, 70));
	DeathSpringArm->TargetArmLength = 0;
	
	DeathCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Death Camera Component"));
	DeathCameraComponent->SetupAttachment(DeathSpringArm);
	DeathCameraComponent->SetAutoActivate(false);
	DeathCameraComponent->SetActive(false);

	Arms_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms FP"));
	Arms_FP->SetupAttachment(CameraComponent);
	Arms_FP->SetOnlyOwnerSee(true);
	Arms_FP->SetRelativeLocation(FVector(-20, 0, -170));
	Arms_FP->SetRelativeRotation(FRotator(0, -90, 0));
	Arms_FP->CastShadow = false;
	Arms_FP->SetCollisionProfileName(COLLISION_ArmsFP, true);

	DeathAnimationSpeed = 200;
	DeathAnimationDistance = 200;
	DeathAnimationRate = 0.01;
	DeathAnimationPitch = -40;
	StopRagdollMovementAfterTime = 2;
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterBase, PC);
	DOREPLIFETIME(ACharacterBase, KillerWeapon);
	DOREPLIFETIME(ACharacterBase, KillForceDirection);
	//DOREPLIFETIME(ACharacterBase, bIsDead);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	if(HealthSystem)
	{
		HealthSystem->OnDie.AddDynamic(this, &ACharacterBase::Server_OnDie);
	}
	
	switch (Team)
	{
		case Attacker:
			Tags.Add(TAG_Attacker);
			break;
		case Defender:
			Tags.Add(TAG_Defender);
		break;
		default:;
	}
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PC = Cast<ABreachersPlayerController>(NewController);
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(MovementSystem) MovementSystem->SetPlayerInputComponent(PlayerInputComponent);
	if(WeaponSystem) WeaponSystem->SetPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(INPUT_OpenBuyMenu, IE_Pressed, this, &ACharacterBase::ShowHideBuyMenu);
	PlayerInputComponent->BindAction(INPUT_Pause, IE_Pressed, this, &ACharacterBase::ShowHidePauseMenu);
	PlayerInputComponent->BindAction(INPUT_ScoreBoard, IE_Pressed, this, &ACharacterBase::OpenScoreBoard);
	PlayerInputComponent->BindAction(INPUT_ScoreBoard, IE_Released, this, &ACharacterBase::CloseScoreBoard);
	PlayerInputComponent->BindAction(INPUT_ChangeTeamMenu, IE_Released, this, &ACharacterBase::ChangeTeam);
}

void ACharacterBase::OnDie()
{
	if(ABreachersGameModeBase* GM = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if(GM->GetIsDestroyWeaponsOnDie()) WeaponSystem->DestroyAllWeapons();
		else WeaponSystem->DropAllWeapons();
	}
	
	Client_OnDie_Visuals();
	Multicast_OnDie_Ragdoll();
	Multicast_PushOnDeath();
}

void ACharacterBase::Server_OnDie_Implementation(AController* InstigatedBy, AActor* DamageCauser)
{
	FTimerHandle OnDieTimer;
	GetWorldTimerManager().SetTimer(OnDieTimer, this, &ACharacterBase::OnDie, 1, false, 0.01);
}

void ACharacterBase::Client_OnDie_Visuals_Implementation()
{
	SwitchToDeathCamera();
	Arms_FP->SetOwnerNoSee(true);
	GetMesh()->SetOwnerNoSee(false);
}

void ACharacterBase::Multicast_OnDie_Ragdoll_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(COLLISION_Ragdoll, true);
	GetCapsuleComponent()->SetCollisionProfileName(COLLISION_Ragdoll, true);

	FTimerHandle PhysicsTimer;
	GetWorldTimerManager().SetTimer(PhysicsTimer, this, &ACharacterBase::StopRagdollMovement, 1, false, StopRagdollMovementAfterTime);
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

void ACharacterBase::SwitchToDeathCamera()
{
	CameraComponent->SetActive(false);
	DeathCameraComponent->SetActive(true);
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ACharacterBase::DeathCameraAnimation, DeathAnimationRate, true);
}

void ACharacterBase::DeathCameraAnimation()
{
	DeathSpringArm->TargetArmLength += DeathAnimationRate * DeathAnimationSpeed;

	FRotator NewDeathCamRotation = DeathCameraComponent->GetRelativeRotation();
	NewDeathCamRotation.Pitch -= DeathAnimationRate * DeathAnimationSpeed / 4;
	if(NewDeathCamRotation.Pitch > DeathAnimationPitch) DeathCameraComponent->SetRelativeRotation(NewDeathCamRotation);
	
	if(DeathSpringArm->TargetArmLength >= DeathAnimationDistance)
	{
		GetWorldTimerManager().ClearTimer(DeathTimerHandle);
	}
}

void ACharacterBase::StopRagdollMovement() const
{
	GetMesh()->SetComponentTickEnabled(false);
}

void ACharacterBase::PushOnDeath(AActor* DamageCauser, FVector ShotFromDirection)
{
	KillerWeapon = DamageCauser;
	KillForceDirection = ShotFromDirection;
}

void ACharacterBase::Multicast_PushOnDeath_Implementation()
{
	if(!KillerWeapon) return;
	if(AWeaponBase* DamageCauser = Cast<AWeaponBase>(KillerWeapon))
	{
		GetMesh()->AddImpulse(KillForceDirection.GetSafeNormal() * DamageCauser->WeaponInfo.DamageInfo.KillPushForce * 3, NAME_None, true);
		LaunchCharacter(KillForceDirection.GetSafeNormal() * DamageCauser->WeaponInfo.DamageInfo.KillPushForce, true, true);
	}
}

void ACharacterBase::ShowHideBuyMenu()
{
	if(!PC) return;
	PC->BuyMenu->ShowHideBuyMenu();
}

void ACharacterBase::ShowHidePauseMenu()
{
	if(!PC) return;
	PC->ShowHidePauseMenu();
}

void ACharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	OnLand.Broadcast();
}

FHitResult ACharacterBase::GetSurfaceType()
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	FVector Start = GetMesh()->GetComponentLocation();
	FVector End = Start;
	End.Z -= 30;
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	return OutHit;
}

void ACharacterBase::OpenScoreBoard()
{
	PC->OpenScoreBoard();
}

void ACharacterBase::CloseScoreBoard()
{
	PC->CloseScoreBoard();
}

void ACharacterBase::ChangeTeam()
{
	if(HealthSystem->GetIsDead()) return;
	PC->ToggleChangeTeamMenu();
}

void ACharacterBase::SetMouseSensitivityFactor(float NewFactor)
{
	Client_SetMouseSensitivityFactor(NewFactor);
}

void ACharacterBase::Client_SetMouseSensitivityFactor_Implementation(float NewFactor)
{
	MovementSystem->SetMouseSensitivityFactor(NewFactor);
}
