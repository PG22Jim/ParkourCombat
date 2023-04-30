// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Player/AN_OnFinishParkourAction.h"

#include "Player/ParkourInterface.h"


void UAN_OnFinishParkourAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);


	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UParkourInterface::StaticClass()))
	{
		// if it has UEnemyRangeInterface, it means its Ranged character, execute its OnRifleBeginAiming function
		IParkourInterface::Execute_OnParkourActionEnd(OwnerRef);
	}

}
