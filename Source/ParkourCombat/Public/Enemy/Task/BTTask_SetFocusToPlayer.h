// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Task/BTTask_BaseActionTask.h"
#include "BTTask_SetFocusToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_SetFocusToPlayer : public UBTTask_BaseActionTask
{
	GENERATED_BODY()

protected:

	UBTTask_SetFocusToPlayer();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
