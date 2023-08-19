// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_OnFinishDecideCommand.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_OnFinishDecideCommand::UBTTask_OnFinishDecideCommand()
{
	NodeName = "Store Decided Command";
	BBKey_DecidingActionCommand.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_OnFinishDecideCommand, BBKey_DecidingActionCommand), StaticEnum<EnemyActionCommand>());
	BBKey_DecidingAttackCategory.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_OnFinishDecideCommand, BBKey_DecidingAttackCategory), StaticEnum<EnemyAttackCategory>());

	
}

void UBTTask_OnFinishDecideCommand::StoreDecidedCommandToReplay(EnemyActionCommand DecidedCommand,
	EnemyAttackCategory DecidedAttack)
{
	switch (DecidedCommand)
	{
		case EnemyActionCommand::Attack:
			if(DecidedAttack == EnemyAttackCategory::ChargeAttack)
				EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::ChargeAttack);
			else if(DecidedAttack == EnemyAttackCategory::LaunchAttack)
				EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::LaunchAttack);
			else if(DecidedAttack == EnemyAttackCategory::NormalAttack)
				EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::NormalAttack);
			else if(DecidedAttack == EnemyAttackCategory::LongRangeAttack)
				EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::LongRangeAttack);
			break;
		case EnemyActionCommand::Idle:
			break;
		case EnemyActionCommand::Counter:
			EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::Counter);

			break;
		case EnemyActionCommand::Heal:
			EnemyRef->UpdateStoredFrameReplayCommand(EEnemyReplayCommand::Heal);
			break;
		default: ;
	}

	
}

EBTNodeResult::Type UBTTask_OnFinishDecideCommand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	

	TryStoreReference(AIOwner->GetPawn());
	if(!EnemyRef) return EBTNodeResult::Failed;
	

	const EnemyActionCommand DecidedAction = static_cast<EnemyActionCommand>(BlackBoard->GetValueAsEnum(BBKey_DecidingActionCommand.SelectedKeyName));
	const EnemyAttackCategory DecidedAttackCategory = static_cast<EnemyAttackCategory>(BlackBoard->GetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName));
	
	StoreDecidedCommandToReplay(DecidedAction, DecidedAttackCategory);

	
	return EBTNodeResult::Succeeded;
}
