#include "Site.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ASite::ASite()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = BoxComponent;
}

void ASite::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASite::OnPlayerEnterSite);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASite::OnPlayerExitSite);
}

void ASite::OnPlayerEnterSite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s"), *OtherActor->GetName()));
}

void ASite::OnPlayerExitSite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s"), *OtherActor->GetName()));
}