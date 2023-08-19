// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "Enemy/EnemyBaseController.h"
#include "BTD_ConditionVerification.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTD_ConditionVerification : public UBTDecorator_Blackboard
{
	GENERATED_BODY()

	AEnemyBaseController* EnemyControllerRef;
	
	
protected:

	UBTD_ConditionVerification();

	bool TryCacheEnemyController(const UBlackboardComponent& Blackboard);

	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) override;
	
	
};
