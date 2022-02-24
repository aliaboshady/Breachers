#include "SpectatorSystem.h"

USpectatorSystem::USpectatorSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpectatorSystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
}

