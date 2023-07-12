// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Player/AN_OnFinishParry.h"

#include "Player/PlayerCombatActionInterface.h"

void UAN_OnFinishParry::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UPlayerCombatActionInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IPlayerCombatActionInterface::Execute_OnEnterSpecificParryState(OwnerRef, ParryStatus::Idle);
	}
}
