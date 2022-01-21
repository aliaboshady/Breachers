#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class USoundCue;
class ACharacterBase;
class UAimOffsetBlendSpace1D;

UCLASS()
class BREACHERS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void PlayFootstepSound();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	USoundCue* GetSurfaceSoundFootstep(FHitResult OutHit);
	USoundCue* GetSurfaceSoundLand(FHitResult OutHit);

	UFUNCTION(Server, Reliable)
	void PlayLandSound();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayFeetSound(USoundCue* SoundCue);
	
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* FootstepSound_Rock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* FootstepSound_Tile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* LandSound_Rock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* LandSound_Tile;
};