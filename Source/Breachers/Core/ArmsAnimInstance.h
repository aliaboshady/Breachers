#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Breachers/Weapons/WeaponInfo.h"
#include "ArmsAnimInstance.generated.h"

class ACharacterBase;

UCLASS()
class BREACHERS_API UArmsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void Client_ChangePose();

	UPROPERTY()
	ACharacterBase* OwnerCharacter;
	
	UPROPERTY(BlueprintReadOnly)
	UAnimSequenceBase* IdlePose_ArmsFP;
};
