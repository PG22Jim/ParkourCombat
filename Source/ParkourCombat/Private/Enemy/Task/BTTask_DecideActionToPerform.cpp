// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_DecideActionToPerform.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyEnum.h"
#include "Kismet/GameplayStatics.h"

UBTTask_DecideActionToPerform::UBTTask_DecideActionToPerform()
{
	NodeName = "Decide Which Action To Perform";
	BBKey_IsConditionChange.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideActionToPerform, BBKey_IsConditionChange));
	BBKey_ConditionChangeAction.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideActionToPerform, BBKey_ConditionChangeAction), StaticEnum<EnemyActionCommand>());
	BBKey_DecidingAction.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideActionToPerform, BBKey_DecidingAction), StaticEnum<EnemyActionCommand>());
	BBKey_ReplayCommand.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideActionToPerform, BBKey_ReplayCommand), StaticEnum<EEnemyReplayCommand>());
	BBKey_Difficulty.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideActionToPerform, BBKey_Difficulty), StaticEnum<EnemyDifficulty>());
}

EBTNodeResult::Type UBTTask_DecideActionToPerform::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;

	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	APawn* OwningPawn = AIOwner->GetPawn();
	if(!OwningPawn) return EBTNodeResult::Failed;

	TryStoreReference(AIOwner->GetPawn());
	if(!EnemyRef) return EBTNodeResult::Failed;

	const FVector OwnPawnPos = OwningPawn->GetActorLocation();
	
	const bool bConditionChange = BlackBoard->GetValueAsBool(BBKey_IsConditionChange.SelectedKeyName);

	// If Condition Changed
	if(bConditionChange)
	{
		const int32 ConditionChangeAction = BlackBoard->GetValueAsEnum(BBKey_ConditionChangeAction.SelectedKeyName);
		BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, ConditionChangeAction);
		BlackBoard->SetValueAsBool(BBKey_IsConditionChange.SelectedKeyName, false);
		BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Idle));
		return EBTNodeResult::Succeeded;
	}
	
	// Debug
	if(IsDebugOn)
	{
		int32 ActionCommandIndex = static_cast<int32>(DebugAction);
		BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, ActionCommandIndex);
		return EBTNodeResult::Succeeded;
	}
	
	// TODO: Real Deciding
	const EEnemyReplayCommand RecordReplayCommand = static_cast<EEnemyReplayCommand>(BlackBoard->GetValueAsEnum(BBKey_ReplayCommand.SelectedKeyName));
	if(RecordReplayCommand != EEnemyReplayCommand::None)
	{
		DecideToExecuteReplayCommand(RecordReplayCommand, BlackBoard);
		return EBTNodeResult::Succeeded;
	}


	// Check if enemy need to heal
	if(DecideToHeal(OwnPawnPos))
	{
		BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Heal));
		return EBTNodeResult::Succeeded;
	}
	
	// if enemy is not healing, then it is attacking
	BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Attack));
	return EBTNodeResult::Succeeded;
}

void UBTTask_DecideActionToPerform::DecideToExecuteReplayCommand(EEnemyReplayCommand ReplayCommand, UBlackboardComponent* BlackBoard)
{
	if(ReplayCommand == EEnemyReplayCommand::Counter)
	{
		BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Counter));
		return;
	}
	
	if(ReplayCommand == EEnemyReplayCommand::Heal)
	{
		BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Heal));
		return;
	}

	
	BlackBoard->SetValueAsEnum(BBKey_DecidingAction.SelectedKeyName, static_cast<int32>(EnemyActionCommand::Attack));
}

bool UBTTask_DecideActionToPerform::DecideToHeal(const FVector OwnPawnPos)
{
	const bool bOwnerHaveHealItem = EnemyRef->AbleToHeal();
	if(!bOwnerHaveHealItem) return false; 

	const bool bOwnerNeedToHeal = EnemyRef->GetCurrentHealthPercentage() < RequestHealPercentageToUseHealItem;
	if(!bOwnerNeedToHeal) return false; 

	
	if(!PlayerCharacterRef)
	{
		ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if(!PlayerCha) return false;
	}
	
	const FVector PlayerPos = PlayerCharacterRef->GetActorLocation();
	const float DistanceToPlayer = UKismetMathLibrary::Vector_Distance(OwnPawnPos, PlayerPos);
	const bool bHaveDistanceToLeaveAndHeal = DistanceToPlayer > UnableToLeaveAndHealDistance;
	if(!bHaveDistanceToLeaveAndHeal) return false; 

	return true;
}


float UBTTask_DecideActionToPerform::InitialChanceByDifficulty(EnemyDifficulty Difficulty)
{
	if(Difficulty == EnemyDifficulty::Easy)
		return HealChanceSetting_Easy;
	if(Difficulty == EnemyDifficulty::Normal)
		return HealChanceSetting_Normal;

	return HealChanceSetting_Hard;
}
