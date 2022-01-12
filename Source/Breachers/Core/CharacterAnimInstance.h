#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class ACharacterBase;
class UAimOffsetBlendSpace1D;

UCLASS()
class BREACHERS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangePose();

	float GetAimOffsetPitch() const;

	UPROPERTY()
	ACharacterBase* OwnerCharacter;
	
	UPROPERTY(BlueprintReadOnly)
	float Speed;

	UPROPERTY(BlueprintReadOnly)
	float Direction;
	
	UPROPERTY(BlueprintReadOnly)
	float AimOffsetPitch;

	UPROPERTY(BlueprintReadOnly)
	bool bIsJumping;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequenceBase* IdlePose_ArmsTP;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAimOffsetBlendSpace1D* BlendSpace_ArmsTP;
};
