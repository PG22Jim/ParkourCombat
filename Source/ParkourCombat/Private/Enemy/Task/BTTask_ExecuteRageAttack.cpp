// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ExecuteRageAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExecuteRageAttack::UBTTask_ExecuteRageAttack()
{
	NodeName = "Perform Rage Attack";
}

EBTNodeResult::Type UBTTask_ExecuteRageAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	

	TryStoreReference(AIOwner->GetPawn());
	
	if(!EnemyRef) return EBTNodeResult::Failed;
	
	EnemyRef->OnEnemyFinishTask.BindDynamic(this, &UBTTask_ExecuteRageAttack::FinishActionTask);
	
	BlackBoard->SetValueAsBool(BBKey_IsExecutingTask.SelectedKeyName, true);
	EnemyRef->SetExecutingTask(true);
	EnemyRef->ExecuteRageAttack();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteRageAttack::FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference, bool DoesGetInterrupted)
{
	EnemyRef->SetExecutingTask(false);
	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}
