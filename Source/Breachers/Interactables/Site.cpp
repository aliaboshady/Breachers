#include "Site.h"
#include "Breachers/Components/PlantDefuseSystem.h"
#include "Components/BoxComponent.h"

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
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->OnPlayerEnterSite();
	}
}

void ASite::OnPlayerExitSite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->OnPlayerExitSite();
	}
}