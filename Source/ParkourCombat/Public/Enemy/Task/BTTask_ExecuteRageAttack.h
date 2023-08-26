// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Task/BTTask_BaseActionTask.h"
#include "BTTask_ExecuteRageAttack.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_ExecuteRageAttack : public UBTTask_BaseActionTask
{
	GENERATED_BODY()
protected:

	UBTTask_ExecuteRageAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference,bool DoesGetInterrupted);
};
