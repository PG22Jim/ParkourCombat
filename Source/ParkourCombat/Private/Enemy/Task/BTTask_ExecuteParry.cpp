// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ExecuteParry.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ExecuteParry::UBTTask_ExecuteParry()
{
	NodeName = "Perform Parry";
}

EBTNodeResult::Type UBTTask_ExecuteParry::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	

	TryStoreReference(AIOwner->GetPawn());
	
	if(!EnemyRef) return EBTNodeResult::Failed;
	
	EnemyRef->OnEnemyFinishTask.BindDynamic(this, &UBTTask_ExecuteParry::FinishActionTask);

	BlackBoard->SetValueAsBool(BBKey_IsExecutingTask.SelectedKeyName, true);
	EnemyRef->ExecuteParry();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteParry::FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference,
	bool DoesGetInterrupted)
{
	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}
