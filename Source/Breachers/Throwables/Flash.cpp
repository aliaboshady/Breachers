#include "Flash.h"

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
	
}
