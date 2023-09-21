// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/AN_OnGameClear.h"

#include "Enemy/EnemyActionInterface.h"

void UAN_OnGameClear::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UEnemyActionInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IEnemyActionInterface::Execute_OnGameClearWhenEnemyDie(OwnerRef);
	}
}
