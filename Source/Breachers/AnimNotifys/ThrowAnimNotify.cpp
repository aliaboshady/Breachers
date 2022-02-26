#include "ThrowAnimNotify.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"

void UThrowAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if(ACharacterBase* CharacterPlayer = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(MeshComp != CharacterPlayer->GetMesh()) return;
		CharacterPlayer->WeaponSystem->Throw();
	}
}
