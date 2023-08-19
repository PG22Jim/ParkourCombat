// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_ExecuteNormalAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyEnum.h"

UBTTask_ExecuteNormalAttack::UBTTask_ExecuteNormalAttack()
{
	NodeName = "Execute Normal Attack";
	BBKey_EnemyDifficulty.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_ExecuteNormalAttack, BBKey_EnemyDifficulty), StaticEnum<EnemyDifficulty>());
}

FNAttackComboResult UBTTask_ExecuteNormalAttack::GetNAttackResultWithDifficulty(AEnemyBase* ExecutingEnemy, EnemyDifficulty CurrentDifficulty)
{
	const int32 MaxNAttackCount = (CurrentDifficulty == EnemyDifficulty::Hard) ? ExecutingEnemy->GetNAttackAnimCount_Long() : ExecutingEnemy->GetNAttackAnimCount_Short();

	
	const int32 DividingChance = 100 / MaxNAttackCount;

	int32 ReturnChargeIndex = MaxNAttackCount;

	int32 RndNum = UKismetMathLibrary::RandomIntegerInRange(1, 100);
	
	if(CurrentDifficulty == EnemyDifficulty::Easy)
	{
		RndNum = UKismetMathLibrary::FClamp(RndNum + NAChanceModify_Easy, 0.0f, 100.0f);
	}
	else if(CurrentDifficulty == EnemyDifficulty::Normal)
	{
		RndNum = UKismetMathLibrary::FClamp(RndNum + NAChanceModify_Normal, 0.0f, 100.0f);
		ReturnChargeIndex = UKismetMathLibrary::RandomIntegerInRange(0, MaxNAttackCount -1);
	}
	else
	{
		if(UKismetMathLibrary::RandomIntegerInRange(1, 100) > 40)
		{
			RndNum = UKismetMathLibrary::FClamp(RndNum + NAChanceModify_Hard, 0.0f, 100.0f);
			ReturnChargeIndex = UKismetMathLibrary::RandomIntegerInRange(MaxNAttackCount / 2, MaxNAttackCount -1);
		}
		else
		{
			RndNum = UKismetMathLibrary::FClamp(RndNum, 0.0f, 100.0f);
			ReturnChargeIndex = UKismetMathLibrary::RandomIntegerInRange(0, MaxNAttackCount -1);
		}
	}	

	const bool bDoesModuloRemain = (RndNum % DividingChance) > 0 ;
	const int32 DividedIndex = RndNum / DividingChance;
	const int32 ReturnNACount = (DividedIndex + bDoesModuloRemain) -1;
	
	return FNAttackComboResult(ReturnNACount,ReturnChargeIndex);
}

void UBTTask_ExecuteNormalAttack::FinishActionTask(UBehaviorTreeComponent* BehaviorTreeReference,
	bool DoesGetInterrupted)
{
	EnemyRef->SetExecutingTask(false);
	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_ExecuteNormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIOwner = OwnerComp.GetAIOwner();
	if(!AIOwner) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;
	
	TryStoreReference(AIOwner->GetPawn());
	
	if(!EnemyRef) return EBTNodeResult::Failed;

	const EnemyDifficulty CurrentEnemyDifficulty = static_cast<EnemyDifficulty>(BlackBoard->GetValueAsEnum(BBKey_EnemyDifficulty.SelectedKeyName)); 
	
	EnemyRef->OnEnemyFinishTask.BindDynamic(this, &UBTTask_ExecuteNormalAttack::FinishActionTask);

	const FNAttackComboResult IndexResult = GetNAttackResultWithDifficulty(EnemyRef, CurrentEnemyDifficulty);

	BlackBoard->SetValueAsBool(BBKey_IsExecutingTask.SelectedKeyName, true);
	
	EnemyRef->SetExecutingTask(true);
	EnemyRef->ExecuteNormalAttack(IndexResult.GetComboCountToExecute(), IndexResult.GetIndexToExecuteChargeAttack());
	
	return EBTNodeResult::InProgress;
}

