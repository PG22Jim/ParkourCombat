// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearAIFocus.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_ClearAIFocus : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	UBTTask_ClearAIFocus() { NodeName = "Clear Controller Focus";}

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
