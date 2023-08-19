// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ExecuteChargeAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExecuteChargeAttack::UBTTask_ExecuteChargeAttack()
{
	NodeName = "Execute Charge Attack";
}

EBTNodeResult::Type UBTTask_ExecuteChargeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	

	TryStoreReference(AIOwner->GetPawn());
	
	if(!EnemyRef) return EBTNodeResult::Failed;
	
	EnemyRef->OnEnemyFinishTask.BindDynamic(this, &UBTTask_ExecuteChargeAttack::FinishActionTask);

	BlackBoard->SetValueAsBool(BBKey_IsExecutingTask.SelectedKeyName, true);
	EnemyRef->ExecuteChargeAttack();
	EnemyRef->SetExecutingTask(true);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteChargeAttack::FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference,
                                                   bool DoesGetInterrupted)
{
	EnemyRef->SetExecutingTask(false);
	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}
