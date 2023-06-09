// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Player/ANS_PlayerOnUpdateMeshPos.h"

#include "Player/ParkourInterface.h"

void UANS_PlayerOnUpdateMeshPos::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UParkourInterface::StaticClass()))
	{
		// if it has UEnemyRangeInterface, it means its Ranged character, execute its OnRifleBeginAiming function
		IParkourInterface::Execute_OnUpdateMeshPosition(OwnerRef);
	}




}
