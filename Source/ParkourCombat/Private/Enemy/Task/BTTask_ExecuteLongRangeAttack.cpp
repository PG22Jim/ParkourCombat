// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ExecuteLongRangeAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExecuteLongRangeAttack::UBTTask_ExecuteLongRangeAttack()
{
	NodeName = "Execute Long Range Attack";
}

EBTNodeResult::Type UBTTask_ExecuteLongRangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	

	TryStoreReference(AIOwner->GetPawn());
	
	if(!EnemyRef) return EBTNodeResult::Failed;
	
	EnemyRef->OnEnemyFinishTask.BindDynamic(this, &UBTTask_ExecuteLongRangeAttack::FinishActionTask);

	BlackBoard->SetValueAsBool(BBKey_IsExecutingTask.SelectedKeyName, true);
	
	EnemyRef->SetExecutingTask(true);

	EnemyRef->ExecuteRangeAttack();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteLongRangeAttack::FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference,
                                                      bool DoesGetInterrupted)
{
	EnemyRef->SetExecutingTask(false);

	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}
