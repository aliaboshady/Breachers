#include "Bomb.h"
#include "Breachers/Components/PlantDefuseSystem.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ABomb::ABomb()
{
	DefuseArea = CreateDefaultSubobject<USphereComponent>("DefuseArea");
	DefuseArea->InitSphereRadius(130);
	DefuseArea->SetupAttachment(RootComponent);
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
	DefuseArea->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnPlayerEnterDefuseArea);
	DefuseArea->OnComponentEndOverlap.AddDynamic(this, &ABomb::OnPlayerExitDefuseArea);
}

void ABomb::OnPlayerEnterDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->SetBombToDefuse(this);
	}
}

void ABomb::OnPlayerExitDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->UnsetBombToDefuse();
	}
}
