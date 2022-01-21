#include "FootstepsAnimNotify.h"
#include "Breachers/AnimInstances/CharacterAnimInstance.h"

void UFootstepsAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if(UCharacterAnimInstance* CharachterAnimInst = Cast<UCharacterAnimInstance>(MeshComp->GetAnimInstance()))
	{
		CharachterAnimInst->PlayFootstepSound();
	}
}
