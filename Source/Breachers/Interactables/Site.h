#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Site.generated.h"

class UBoxComponent;

UCLASS()
class BREACHERS_API ASite : public AActor
{
	GENERATED_BODY()
	
public:	
	ASite();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxComponent;

	UFUNCTION()
	void OnPlayerEnterSite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnPlayerExitSite(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
