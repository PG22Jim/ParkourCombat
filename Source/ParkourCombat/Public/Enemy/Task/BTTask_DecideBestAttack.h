// Copyright © 2023 by Jim Chen. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActionTask.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy/EnemyEnum.h"
#include "BTTask_DecideBestAttack.generated.h"

/**
 * 
 */
UCLASS()
class PARKOURCOMBAT_API UBTTask_DecideBestAttack : public UBTTask_BaseActionTask
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_DecidingAttackCategory;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToModify")
	FBlackboardKeySelector BBKey_ConditionChangeAttackCategory;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_DistanceToPlayer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_EnemyDifficulty;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= "ValueToGrab")
	FBlackboardKeySelector BBKey_ReplayCommand;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= DEBUG)
	EnemyAttackCategory DebugAttack = EnemyAttackCategory::ChargeAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= DEBUG)
	bool IsDebugOn = false;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_DistanceClassification)
	float DistanceForCloseRange = 400.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_DistanceClassification)
	float DistanceForMidRange = 700.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_DistanceClassification)
	float DistanceForLongRange = 1000.0f;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToLaunchAttackInClose_Hard = 33;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToLongRangeAttackInClose_Hard = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToChargeAttackInClose_Hard = 33;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToLaunchAttackInClose_Normal = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToLongRangeAttackInClose_Normal = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Close)
	float ChanceToChargeAttackInClose_Normal = 20;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToLaunchAttackInMid_Hard = 30;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToNormalAttackInMid_Hard = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToLongRangeAttackInMid_Hard = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToLaunchAttackInMid_Normal = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToNormalAttackInMid_Normal = 20;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Mid)
	float ChanceToLongRangeAttackInMid_Normal = 20;
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToLaunchAttackInLong_Hard = 25;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToNormalAttackInLong_Hard = 25;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToLongRangeAttackInLong_Hard = 25;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToLaunchAttackInLong_Normal = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToNormalAttackInLong_Normal = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= TaskSetting_AttackChance_Long)
	float ChanceToLongRangeAttackInLong_Normal = 20;
	
	
	UBTTask_DecideBestAttack();

	EnemyAttackCategory DecideBestAttackBySituations(float DistanceToPlayer, EnemyDifficulty CurrentDifficulty);
	bool DecideToUseRageAttack(EnemyDifficulty CurrentDifficulty);
	bool TryDecideReplayCommand(UBlackboardComponent* BlackBoard);
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
