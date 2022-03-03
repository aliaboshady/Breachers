#include "Flash.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/FlashComponent.h"
#include "Breachers/PlayerControllers/PlantAndDefusePlayerController.h"
#include "Kismet/GameplayStatics.h"

AFlash::AFlash()
{
	MaxFlashDistance = 3000;
	FullFlashTime = 2;
	HalfFlashTime = 0.5;
	FullFlashAmount = 1;
	HalfFlashAmount = 0.3;
	FullFlashAngle = 0;
	FullFlashFadeTime = 1;
	HalfFlashFadeTime = 0.2;
}

void AFlash::OnThrow()
{
	Super::OnThrow();
	FTimerHandle ActivateTimer;
	GetWorldTimerManager().SetTimer(ActivateTimer, this, &AFlash::OnActivate, 1, false, TimeToActivate);
}

void AFlash::OnActivate()
{
	Server_Flash();
	Super::OnActivate();
}

void AFlash::Server_Flash_Implementation()
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Player, AllPlayers);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	FVector Start = GetActorLocation();
	
	for (AActor* Player : AllPlayers)
	{
		ACharacterBase* CharacterBasePlayer = Cast<ACharacterBase>(Player);
		if(!CharacterBasePlayer) continue;
		
		FVector PlayerLocation = CharacterBasePlayer->GetCameraLocation();
		FVector End = CharacterBasePlayer->GetCameraLocation() + (PlayerLocation - Start).GetSafeNormal() * 50;
		FHitResult OutHit;
		UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
		if(!OutHit.bBlockingHit || !OutHit.Actor->ActorHasTag(TAG_Player) || OutHit.Distance > MaxFlashDistance) continue;

		FVector PlayerLookDirection = CharacterBasePlayer->GetControlRotation().Vector();
		FVector FlashToPlayerDirection = End - Start;
		float FacingAngle = -FVector::DotProduct(FlashToPlayerDirection.GetSafeNormal(), PlayerLookDirection.GetSafeNormal());

		if(APlantAndDefusePlayerController* PDPC = Cast<APlantAndDefusePlayerController>(CharacterBasePlayer->GetBreacherPC()))
		{
			if(FacingAngle > FullFlashAngle) PDPC->FlashComponent->GetFlashed(FullFlashAmount, FullFlashTime, FullFlashFadeTime, true);
			else PDPC->FlashComponent->GetFlashed(HalfFlashAmount, HalfFlashTime, HalfFlashFadeTime, false);
		}
	}
}