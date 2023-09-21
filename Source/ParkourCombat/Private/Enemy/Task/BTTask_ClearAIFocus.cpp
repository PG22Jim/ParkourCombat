// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ClearAIFocus.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type UBTTask_ClearAIFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwnerController = OwnerComp.GetAIOwner();
	if(!AIOwnerController) return EBTNodeResult::Failed;
	
	AIOwnerController->ClearFocus(EAIFocusPriority::Gameplay);
	
	return EBTNodeResult::Succeeded;
}
