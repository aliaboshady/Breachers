#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "EquipWeaponAnimNotify.generated.h"

class AWeaponBase;

UCLASS()
class BREACHERS_API UEquipWeaponAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UEquipWeaponAnimNotify();

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
	bool bClientOnlySound;
};
