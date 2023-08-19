// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Decorator/BTD_ConditionVerification.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTD_ConditionVerification::UBTD_ConditionVerification()
{
	NodeName = "BBCondition_CanBeInterrupted";
}

bool UBTD_ConditionVerification::TryCacheEnemyController(const UBlackboardComponent& Blackboard)
{
	if(EnemyControllerRef) return true;
	
	AActor* OwnerActor = Blackboard.GetOwner();
	if(!OwnerActor) return false;

	AEnemyBaseController* CastedResult = Cast<AEnemyBaseController>(OwnerActor);
	if(!CastedResult) return false;

	EnemyControllerRef = CastedResult;
	return true;
}

EBlackboardNotificationResult UBTD_ConditionVerification::OnBlackboardKeyValueChange(
	const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	if(TryCacheEnemyController(Blackboard)) EnemyControllerRef->OnConditionChangeInterrupt();
	
	return Super::OnBlackboardKeyValueChange(Blackboard, ChangedKeyID);
}
