// Copyright © 2023 by Jim Chen. All rights reserved.


#include "Enemy/Task/BTTask_DecideBestAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_DecideBestAttack::UBTTask_DecideBestAttack()
{
	NodeName = "Decide Which Is Best Attack Choice";

	BBKey_DecidingAttackCategory.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideBestAttack, BBKey_DecidingAttackCategory), StaticEnum<EnemyAttackCategory>());
	BBKey_ConditionChangeAttackCategory.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideBestAttack, BBKey_ConditionChangeAttackCategory), StaticEnum<EnemyAttackCategory>());
	BBKey_EnemyDifficulty.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideBestAttack, BBKey_EnemyDifficulty), StaticEnum<EnemyDifficulty>());
	BBKey_DistanceToPlayer.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideBestAttack, BBKey_DistanceToPlayer));
	BBKey_ReplayCommand.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DecideBestAttack, BBKey_ReplayCommand), StaticEnum<EEnemyReplayCommand>());
}

EnemyAttackCategory UBTTask_DecideBestAttack::DecideBestAttackBySituations(float DistanceToPlayer, EnemyDifficulty CurrentDifficulty)
{
	if(IsDebugOn)
		return DebugAttack;
	
	const float RndFloat = UKismetMathLibrary::RandomFloatInRange(1.0f,100.0f);

	// Close Range
	if(DistanceToPlayer < DistanceForCloseRange)
	{
		// Enemy Difficulty is Normal
		if(CurrentDifficulty == EnemyDifficulty::Normal)
		{
			if(RndFloat < (100.0f - ChanceToLaunchAttackInClose_Normal - ChanceToChargeAttackInClose_Normal - ChanceToLongRangeAttackInClose_Normal))
				return EnemyAttackCategory::NormalAttack;

			if(RndFloat < (100.0f - ChanceToChargeAttackInClose_Normal - ChanceToChargeAttackInClose_Normal))
				return EnemyAttackCategory::LaunchAttack;

			if(RndFloat < (100.0f - ChanceToChargeAttackInClose_Normal))
				return EnemyAttackCategory::LongRangeAttack;
			
			return EnemyAttackCategory::ChargeAttack;
		}

		// Enemy Difficulty is Hard
		if(CurrentDifficulty == EnemyDifficulty::Hard)
		{
			if(RndFloat < (100.0f - ChanceToLaunchAttackInClose_Hard - ChanceToChargeAttackInClose_Hard - ChanceToLongRangeAttackInClose_Hard))
				return EnemyAttackCategory::NormalAttack;

			if(RndFloat < (100.0f - ChanceToChargeAttackInClose_Hard - ChanceToChargeAttackInClose_Hard))
				return EnemyAttackCategory::LaunchAttack;

			if(RndFloat < (100.0f - ChanceToChargeAttackInClose_Hard))
				return EnemyAttackCategory::LongRangeAttack;
			
			return EnemyAttackCategory::ChargeAttack;
		}

		// Enemy Difficulty is Easy, only do normal attack when enemy is in close Range
		return EnemyAttackCategory::NormalAttack;
	}

	// Mid Range
	if(DistanceToPlayer < DistanceForMidRange)
	{
		// Enemy Difficulty is Normal
		if(CurrentDifficulty == EnemyDifficulty::Normal)
		{
			if(RndFloat < (100.0f - ChanceToLaunchAttackInMid_Normal - ChanceToLongRangeAttackInMid_Normal - ChanceToNormalAttackInMid_Normal))
				return EnemyAttackCategory::ChargeAttack;

			if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Normal - ChanceToLongRangeAttackInMid_Normal))
				return EnemyAttackCategory::LaunchAttack;
			
			if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Normal))
				return EnemyAttackCategory::NormalAttack;
			
			return EnemyAttackCategory::LongRangeAttack;
		}

		// Enemy Difficulty is Hard
		if(CurrentDifficulty == EnemyDifficulty::Hard)
		{
			if(RndFloat < (100.0f - ChanceToLaunchAttackInMid_Hard - ChanceToLongRangeAttackInMid_Hard - ChanceToNormalAttackInMid_Hard))
				return EnemyAttackCategory::ChargeAttack;

			if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Hard - ChanceToLongRangeAttackInMid_Hard))
				return EnemyAttackCategory::LaunchAttack;

			if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Hard))
				return EnemyAttackCategory::NormalAttack;
			
			return EnemyAttackCategory::LongRangeAttack;
		}
		
		return EnemyAttackCategory::LaunchAttack;
	}


	// Long Range

	// Enemy Difficulty is Normal
	if(CurrentDifficulty == EnemyDifficulty::Normal)
	{
		if(RndFloat < (100.0f - ChanceToLaunchAttackInMid_Normal - ChanceToLongRangeAttackInMid_Normal - ChanceToNormalAttackInLong_Normal))
			return EnemyAttackCategory::ChargeAttack;

		if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Normal - ChanceToLongRangeAttackInMid_Normal))
			return EnemyAttackCategory::LaunchAttack;

		if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Normal))
			return EnemyAttackCategory::NormalAttack;
		
		return EnemyAttackCategory::LongRangeAttack;
	}

	// Enemy Difficulty is Hard
	if(CurrentDifficulty == EnemyDifficulty::Hard)
	{
		if(RndFloat < (100.0f - ChanceToLaunchAttackInMid_Hard - ChanceToLongRangeAttackInMid_Hard - ChanceToNormalAttackInLong_Hard))
			return EnemyAttackCategory::ChargeAttack;

		if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Hard - ChanceToLongRangeAttackInMid_Hard))
			return EnemyAttackCategory::LaunchAttack;

		if(RndFloat < (100.0f - ChanceToLongRangeAttackInMid_Hard))
			return EnemyAttackCategory::NormalAttack;
			
		return EnemyAttackCategory::LongRangeAttack;
	}
	
	return EnemyAttackCategory::LaunchAttack;
}

bool UBTTask_DecideBestAttack::TryDecideReplayCommand(UBlackboardComponent* BlackBoard)
{
	const EEnemyReplayCommand RecordReplayCommand = static_cast<EEnemyReplayCommand>(BlackBoard->GetValueAsEnum(BBKey_ReplayCommand.SelectedKeyName));
	const bool bRecordAttackReplayCommand = (RecordReplayCommand == EEnemyReplayCommand::NormalAttack || RecordReplayCommand == EEnemyReplayCommand::ChargeAttack || RecordReplayCommand == EEnemyReplayCommand::LaunchAttack || RecordReplayCommand == EEnemyReplayCommand::LongRangeAttack);
	if(bRecordAttackReplayCommand)
	{
		if(RecordReplayCommand == EEnemyReplayCommand::NormalAttack)
		{
			BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::NormalAttack));
			return true;
		}
		
		if(RecordReplayCommand == EEnemyReplayCommand::ChargeAttack)
		{
			BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::ChargeAttack));
			return true;
		}
		if(RecordReplayCommand == EEnemyReplayCommand::LaunchAttack)
		{
			BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::LaunchAttack));
			return true;
		}

		BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::LongRangeAttack));
		return true;
	}

	return false;
}

EBTNodeResult::Type UBTTask_DecideBestAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	if(TryDecideReplayCommand(BlackBoard))
	{
		return EBTNodeResult::Succeeded;
	}
	

	const float DistanceToPlayer = BlackBoard->GetValueAsFloat(BBKey_DistanceToPlayer.SelectedKeyName);

	const EnemyDifficulty CurrentDifficulty = static_cast<EnemyDifficulty>(BlackBoard->GetValueAsEnum(BBKey_EnemyDifficulty.SelectedKeyName));

	const EnemyAttackCategory ConditionChangeCategory = static_cast<EnemyAttackCategory>(BlackBoard->GetValueAsEnum(BBKey_ConditionChangeAttackCategory.SelectedKeyName));
	if(ConditionChangeCategory == EnemyAttackCategory::None)
	{
		const EnemyAttackCategory DecidedAttack = DecideBestAttackBySituations(DistanceToPlayer, CurrentDifficulty);
		BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(DecidedAttack));
	}
	else
		BlackBoard->SetValueAsEnum(BBKey_DecidingAttackCategory.SelectedKeyName, static_cast<uint8>(ConditionChangeCategory));


	BlackBoard->SetValueAsEnum(BBKey_ConditionChangeAttackCategory.SelectedKeyName, static_cast<uint8>(EnemyAttackCategory::None));
	
	return EBTNodeResult::Succeeded;
}
