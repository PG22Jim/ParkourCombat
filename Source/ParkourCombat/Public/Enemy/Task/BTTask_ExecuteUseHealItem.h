// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Task/BTTask_BaseActionTask.h"
#include "BTTask_ExecuteUseHealItem.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_ExecuteUseHealItem : public UBTTask_BaseActionTask
{
	GENERATED_BODY()
protected:

	UBTTask_ExecuteUseHealItem();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference, bool DoesGetInterrupted);
	
};
